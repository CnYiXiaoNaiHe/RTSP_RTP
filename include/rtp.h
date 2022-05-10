#pragma once

#include <stdint.h>
#include <time.h>


#define RTP_VESION              2

#define RTP_PAYLOAD_TYPE_H264   96
#define RTP_PAYLOAD_TYPE_AAC    97

#define RTP_HEADER_SIZE         20
#define RTP_MAX_PKT_SIZE        1400


/*
 *
 *    0                   1                   2                   3
 *    7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                           timestamp                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |           synchronization source (SSRC) identifier            |
 *   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *   |            contributing source (CSRC) identifiers             |
 *   :                             ....                              :
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */


#pragma pack (push)

#pragma pack (1)

struct RtpHeader
{
	/* byte 0 */
	uint8_t csrcLen : 4;  //ָʾCSRC��ʶ������ 
	uint8_t extension : 1; //���X = 1������RTP��ͷ�����һ����չ��ͷ
	uint8_t padding : 1;  //���P=1�����ڸñ��ĵ�β�����һ����������İ�λ�飬���ǲ�����Ч�غɵ�һ����
	uint8_t version : 2;  //��ǰЭ��汾��Ϊ2

	/* byte 1 */
	uint8_t payloadType : 7;//����˵��RTP��������Ч�غɵ�����
	uint8_t marker : 1;//��ͬ����Ч�غ��в�ͬ�ĺ��壬������Ƶ�����һ֡�Ľ�����������Ƶ����ǻỰ�Ŀ�ʼ

	/* bytes 2,3 */
	uint16_t seq;//���ڱ�ʶ�����������͵�RTP���ĵ����к�

	/* bytes 4-7 */
	uint32_t timestamp;//����ʹ��90kHZʱ��Ƶ�ʣ������е�90000��

	/* bytes 8-11 */
	uint32_t ssrc;//���ڱ�ʶͬ����Դ

	/*byte   12-19*/
	uint64_t ntptime;
};



struct RtpPacket
{
	struct RtpHeader rtpHeader;
	uint8_t payload[0];
};

#pragma pack (pop)

class rtp
{
public:
	static void rtpHeaderInit(struct RtpPacket* rtpPacket, uint8_t csrcLen, uint8_t extension,
		uint8_t padding, uint8_t version, uint8_t payloadType, uint8_t marker,
		uint16_t seq, uint32_t timestamp, uint32_t ssrc);
	static int rtpSendPacket(int socket, const char* ip, int16_t port, struct RtpPacket* rtpPacket, uint32_t dataSize);
};




