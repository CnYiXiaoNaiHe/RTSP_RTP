#pragma once
#include"rtp.h"
#include<vector>
#include<queue>


#include <winsock2.h>  
#include <Windows.h>  


#define PACKET_BUFFER_END            (unsigned int)0x00000000


#define MAX_RTP_PKT_LENGTH     1400

#define DEST_IP                "127.0.0.1"
#define DEST_PORT              2345

#define H264                   96

typedef struct
{
	/**//* byte 0 */
	unsigned char csrc_len : 4;        /**//* expect 0 */
	unsigned char extension : 1;        /**//* expect 1, see RTP_OP below */
	unsigned char padding : 1;        /**//* expect 0 */
	unsigned char version : 2;        /**//* expect 2 */
	/**//* byte 1 */
	unsigned char payload : 7;        /**//* RTP_PAYLOAD_RTSP */
	unsigned char marker : 1;        /**//* expect 1 */
	/**//* bytes 2, 3 */
	unsigned short seq_no;
	/**//* bytes 4-7 */
	unsigned  long timestamp;
	/**//* bytes 8-11 */
	unsigned long ssrc;            /**//* stream number is used here. */
} RTP_FIXED_HEADER;

typedef struct {
	//byte 0
	unsigned char TYPE : 5;
	unsigned char NRI : 2;
	unsigned char F : 1;

} NALU_HEADER; /**//* 1 BYTES */

typedef struct {
	//byte 0
	unsigned char TYPE : 5;
	unsigned char NRI : 2;
	unsigned char F : 1;


} FU_INDICATOR; /**//* 1 BYTES */

typedef struct {
	//byte 0
	unsigned char TYPE : 5;
	unsigned char R : 1;
	unsigned char E : 1;
	unsigned char S : 1;
} FU_HEADER; /**//* 1 BYTES */


typedef struct
{
	int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
	unsigned max_size;            //! Nal Unit Buffer size
	int forbidden_bit;            //! should be always FALSE
	int nal_reference_idc;        //! NALU_PRIORITY_xxxx
	int nal_unit_type;            //! NALU_TYPE_xxxx    
	char *buf;                    //! contains the first byte followed by the EBSP
	unsigned short lost_packets;  //! true, if packet loss is detected
} NALU_t;



class rtp_h264
{
public:

	rtp_h264();

	~rtp_h264();

	int rtpSendH264Frame(int socket, char* ip, int16_t port,
		struct RtpPacket* rtpPacket, uint8_t* frame, uint32_t frameSize);

	int find_nal_prefix(char* address, int len, std::vector<uint8_t>&nalBytes);

	int GetNALU(char* address, int len);

	void doClient(char* address, int len);

	void InitNetwork();



private:

	struct RtpPacket* rtpPacket;
	std::vector<uint8_t>m_nalVec;
	static long flag;
	static bool isFirstNALU;

	//????????????
	SOCKET socket1;
    struct sockaddr_in server;
	uint8_t * frame; 
	sockaddr_in clientAddr;

};

