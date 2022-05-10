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
	uint8_t csrcLen : 4;  //指示CSRC标识符个数 
	uint8_t extension : 1; //如果X = 1，则在RTP报头后跟有一个扩展报头
	uint8_t padding : 1;  //如果P=1，则在该报文的尾部填充一个或多个额外的八位组，它们不是有效载荷的一部分
	uint8_t version : 2;  //当前协议版本号为2

	/* byte 1 */
	uint8_t payloadType : 7;//用于说明RTP报文中有效载荷的类型
	uint8_t marker : 1;//不同的有效载荷有不同的含义，对于视频，标记一帧的结束；对于音频，标记会话的开始

	/* bytes 2,3 */
	uint16_t seq;//用于标识发送者所发送的RTP报文的序列号

	/* bytes 4-7 */
	uint32_t timestamp;//必须使用90kHZ时钟频率（程序中的90000）

	/* bytes 8-11 */
	uint32_t ssrc;//用于标识同步信源

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




