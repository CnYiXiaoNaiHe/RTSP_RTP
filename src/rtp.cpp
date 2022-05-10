#include "rtp.h"
#include <winsock2.h>  
#include <Windows.h>  
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")  
/*
 * 作者：一笑奈何
 * 博客：cn-yixiaonaihe.blog.csdn.net
 */


void rtp::rtpHeaderInit(struct RtpPacket* rtpPacket, uint8_t csrcLen, uint8_t extension,
	uint8_t padding, uint8_t version, uint8_t payloadType, uint8_t marker,
	uint16_t seq, uint32_t timestamp, uint32_t ssrc)
{
	rtpPacket->rtpHeader.csrcLen = csrcLen;
	rtpPacket->rtpHeader.extension = extension;
	rtpPacket->rtpHeader.padding = padding;
	rtpPacket->rtpHeader.version = version;
	rtpPacket->rtpHeader.payloadType = payloadType;
	rtpPacket->rtpHeader.marker = marker;
	rtpPacket->rtpHeader.seq = seq;
	rtpPacket->rtpHeader.timestamp = timestamp;
	rtpPacket->rtpHeader.ssrc = ssrc;
}
/*
 * 函数功能：发送RTP包
 * 参数 socket：表示本机的udp套接字
 * 参数 ip：表示目的ip地址
 * 参数 port：表示目的的端口号
 * 参数 rtpPacket：表示rtp包
 * 参数 dataSize：表示rtp包中载荷的大小
 * 放回值：发送字节数
 */

int rtp::rtpSendPacket(int socket, const char* ip, int16_t port, struct RtpPacket* rtpPacket, uint32_t dataSize)
{
	struct sockaddr_in addr;
	int ret;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);


	rtpPacket->rtpHeader.seq = rtpPacket->rtpHeader.seq;
	rtpPacket->rtpHeader.timestamp = rtpPacket->rtpHeader.timestamp;
	rtpPacket->rtpHeader.ssrc = rtpPacket->rtpHeader.ssrc;
	rtpPacket->rtpHeader.ntptime = rtpPacket->rtpHeader.ntptime;

	printf("ServerSender ntptime is %lld\n", rtpPacket->rtpHeader.ntptime);
	ret = sendto(socket, (const char*)rtpPacket, dataSize + RTP_HEADER_SIZE, 0,
		(struct sockaddr*)&addr, sizeof(addr));

	return ret;
}
