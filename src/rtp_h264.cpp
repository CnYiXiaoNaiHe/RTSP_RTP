#include "rtp_h264.h"


#pragma comment(lib,"ws2_32.lib")  

long rtp_h264::flag = 0;
bool rtp_h264::isFirstNALU = true;

static uint64_t GetTime();

rtp_h264::rtp_h264()
{
	int Error;
	WORD VersionRequested;
	WSADATA WsaData;
	VersionRequested = MAKEWORD(2, 2);
	Error = WSAStartup(VersionRequested, &WsaData); //����WinSock2
	if (Error != 0)
	{
		printf("WSAStartup Error\n");
	}
	else
	{
		if (LOBYTE(WsaData.wVersion) != 2 || HIBYTE(WsaData.wHighVersion) != 2)
		{
			WSACleanup();

		}
	}

}

rtp_h264::~rtp_h264()
{
	WSACleanup();
}

int rtp_h264::rtpSendH264Frame(int socket, char * ip, int16_t port, RtpPacket * rtpPacket, uint8_t * frame, uint32_t frameSize)
{
	uint8_t naluType; // nalu��һ���ֽ�
	int sendBytes = 0;
	int ret;

	naluType = frame[0];

	if (frameSize <= RTP_MAX_PKT_SIZE) // nalu����С������������һNALU��Ԫģʽ
	{
		/*
		 *   0 1 2 3 4 5 6 7 8 9
		 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 *  |F|NRI|  Type   | a single NAL unit ... |
		 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 */
		memcpy(rtpPacket->payload, frame, frameSize);
		uint64_t timies = rtpPacket->rtpHeader.ntptime = GetTime();
		printf("time is %lld\n", timies);
		ret = rtp::rtpSendPacket(socket, ip, port, rtpPacket, frameSize);
		if (ret < 0)
			return -1;
		printf("rtpPacket->seqnumber is %d\n", rtpPacket->rtpHeader.seq);
		rtpPacket->rtpHeader.seq++;
		sendBytes += ret;
		if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // �����SPS��PPS�Ͳ���Ҫ��ʱ���
			goto out;
	}
	else // nalu����С������������Ƭģʽ
	{
		/*
		 *  0                   1                   2
		 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
		 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 * | FU indicator  |   FU header   |   FU payload   ...  |
		 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 */

		 /*
		  *     FU Indicator
		  *    0 1 2 3 4 5 6 7
		  *   +-+-+-+-+-+-+-+-+
		  *   |F|NRI|  Type   |
		  *   +---------------+
		  */

		  /*
		   *      FU Header
		   *    0 1 2 3 4 5 6 7
		   *   +-+-+-+-+-+-+-+-+
		   *   |S|E|R|  Type   |
		   *   +---------------+
		   */

		int pktNum = frameSize / RTP_MAX_PKT_SIZE;       // �м��������İ�
		int remainPktSize = frameSize % RTP_MAX_PKT_SIZE; // ʣ�಻�������Ĵ�С
		int i, pos = 1;

		/* ���������İ� */
		for (i = 0; i < pktNum; i++)
		{
			rtpPacket->payload[0] = (naluType & 0x60) | 28;
			rtpPacket->payload[1] = naluType & 0x1F;

			if (i == 0) //��һ������
				rtpPacket->payload[1] |= 0x80; // start
			else if (remainPktSize == 0 && i == pktNum - 1) //���һ������
				rtpPacket->payload[1] |= 0x40; // end

			memcpy(rtpPacket->payload + 2, frame + pos, RTP_MAX_PKT_SIZE);
			
			rtpPacket->rtpHeader.ntptime = GetTime();
			ret = rtp::rtpSendPacket(socket, ip, port, rtpPacket, RTP_MAX_PKT_SIZE + 2);
			if (ret < 0)
				return -1;

			rtpPacket->rtpHeader.seq++;
			sendBytes += ret;
			pos += RTP_MAX_PKT_SIZE;
		}

		/* ����ʣ������� */
		if (remainPktSize > 0)
		{
			rtpPacket->payload[0] = (naluType & 0x60) | 28;
			rtpPacket->payload[1] = naluType & 0x1F;
			rtpPacket->payload[1] |= 0x40; //end

			memcpy(rtpPacket->payload + 2, frame + pos, remainPktSize + 2);
			
			rtpPacket->rtpHeader.ntptime = GetTime();
			ret = rtp::rtpSendPacket(socket, ip, port, rtpPacket, remainPktSize + 2);
			if (ret < 0)
				return -1;

			rtpPacket->rtpHeader.seq++;
			sendBytes += ret;
		}
	}

out:

	return sendBytes;

}


int rtp_h264::find_nal_prefix(char* address, int len, std::vector<uint8_t>&nalBytes)
{
	if (!address)
	{
		return -1;
	}

	uint8_t prefix[3] = { 0 };
	uint8_t fileByte;


	nalBytes.clear();

	int pos = 0, getPrefix = 0;
	for (int idx = 0; idx < 3; idx++)
	{
		//ÿ�δ��ļ��ж�ȡһ���ֽڴ洢��vector��
		prefix[idx] = address[flag + idx];
		nalBytes.push_back(prefix[idx]);
	}
	flag += 3;
	while (flag < len)
	{
		if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 1))
		{
			//0x 00 00 01
			getPrefix = 1;
			nalBytes.pop_back();
			nalBytes.pop_back();
			nalBytes.pop_back();
			break;
		}
		else if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 0))
		{
			//0x 00 00 00 01
			if (address[flag] == 1)
			{
				getPrefix = 2;
				nalBytes.pop_back();
				nalBytes.pop_back();
				nalBytes.pop_back();
				flag++;
				break;
			}
		}
		else
		{
			//ȡ����һ���ֽ�
			fileByte = address[flag++];
			prefix[(pos++) % 3] = fileByte;
			nalBytes.push_back(fileByte);
		}

	}
	return getPrefix;
}


int rtp_h264::GetNALU(char* address, int len)
{
	if (isFirstNALU)
	{
		find_nal_prefix(address, len, m_nalVec);
		find_nal_prefix(address, len, m_nalVec);
		//for (int i = 0; i < m_nalVec.size(); i++)
		//{
		//	printf("%X ", m_nalVec.at(i));
		//}
		//printf("\n");
		printf("NALU size is %d\n", m_nalVec.size());
		isFirstNALU = false;
	}
	else
	{
		find_nal_prefix(address, len, m_nalVec);
		//for (int i = 0; i < m_nalVec.size(); i++)
		//{
		//	printf("%X ", m_nalVec.at(i));
		//}
		//printf("\n");
		printf("NALU size is %d\n", m_nalVec.size());
	}

	
	return m_nalVec.size();
}

void rtp_h264::doClient(char* address, int addresslen)
{

	while(flag < addresslen)
	{
		int frameSize = GetNALU(address, addresslen);
		for (int i = 0; i < m_nalVec.size(); i++)
		{
			frame[i] = m_nalVec[i];
		}

		rtpSendH264Frame(socket1, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port),
			rtpPacket, frame, frameSize);

		rtpPacket->rtpHeader.timestamp += 90000 / 15;
	}
	flag = 0;
	isFirstNALU = true;

}

void rtp_h264::InitNetwork()
{
	int len = sizeof(server);
	float framerate = 15;
	unsigned int timestamp_increse = 0, ts_current = 0;
	timestamp_increse = (unsigned int)(90000.0 / framerate); //+0.5);  //ʱ�����H264����Ƶ���ó�90000

	server.sin_family = AF_INET;
	server.sin_port = htons(DEST_PORT);
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	socket1 = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket1 == SOCKET_ERROR)
	{
		printf("create socket failed\n");
		return;
	}
	//connect(socket1, (const sockaddr *)&server, len);//����UDP�׽���
	//bind(socket�����֣� �󶨸�listenfd��Э���ַ����ַ����)
	bind(socket1, (sockaddr*)&server, sizeof(sockaddr));

	//Step5����ͻ��˽���ͨ��
	char buf[512];

	memset(buf, 0, 512);

	memset(&clientAddr, 0, sizeof(sockaddr_in));

	int clientAddrLen = sizeof(sockaddr);
	//���տͻ��˷��������� 
	//recvfrom������socket���ƣ��������ݵĻ���������������С����־λ�����ò�����ʽ����sockaddr�ṹ��ַ��sockaddr�ṹ��С��ַ
	//sockaddr��ַ������������﷢�����ͷ��͵�����ĵ�ַ��Ϣ
	int ret = recvfrom(socket1, buf, 512, 0, (sockaddr*)&clientAddr, &clientAddrLen);

	//inet_ntoa����ת��Ϊip��ntohs����ת��Ϊ�˿ں�
	printf("Recv msg:%s from IP:[%s] Port:[%d]\n", buf, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

	//--------------

	frame = (uint8_t *)malloc(500000);
	rtpPacket = (struct RtpPacket*)malloc(500000);
	rtp::rtpHeaderInit(rtpPacket, 0, 0, 0, RTP_VESION, RTP_PAYLOAD_TYPE_H264, 0,
		0, 0, 0x88923423);
}


static uint64_t  GetTime()
{
	time_t tt;
	struct tm *st;
	time(&tt);
	SYSTEMTIME t1;
	GetSystemTime(&t1);
	return tt * 1000 + (uint32_t)t1.wMilliseconds;
	return (uint32_t)t1.wMilliseconds;
}