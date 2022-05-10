#pragma once
#include<stdint.h>

class RtpPacketHistory
{
public:
	// Maximum number of packets we ever allow in the history.
    //��ʷ�������������ݰ�����
	static constexpr size_t kMaxCapacity = 9600;
	// Don't remove packets within max(1000ms, 3x RTT).
	//��Ҫ�����1000ms��3x RTT����Χ��ɾ�����ݰ���
	static constexpr int64_t kMinPacketDurationMs = 1000;
	static constexpr int kMinPacketDurationRtt = 3;
	// With kStoreAndCull, always remove packets after 3x max(1000ms, 3x rtt).
	//ʹ��kStoreAndCull�����������3����1000���룬3��rtt����ɾ�����ݰ���
	static constexpr int kPacketCullingDelayFactor = 3;


	/*void PutRtpPacket(RtpPacketToSend packet);*/


private:


};

