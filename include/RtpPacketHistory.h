#pragma once
#include<stdint.h>

class RtpPacketHistory
{
public:
	// Maximum number of packets we ever allow in the history.
    //历史上允许的最大数据包数。
	static constexpr size_t kMaxCapacity = 9600;
	// Don't remove packets within max(1000ms, 3x RTT).
	//不要在最大（1000ms，3x RTT）范围内删除数据包。
	static constexpr int64_t kMinPacketDurationMs = 1000;
	static constexpr int kMinPacketDurationRtt = 3;
	// With kStoreAndCull, always remove packets after 3x max(1000ms, 3x rtt).
	//使用kStoreAndCull，总是在最大3倍（1000毫秒，3倍rtt）后删除数据包。
	static constexpr int kPacketCullingDelayFactor = 3;


	/*void PutRtpPacket(RtpPacketToSend packet);*/


private:


};

