#pragma once
#include"rtp_rtcp_defines.h"

class RtpPacket
{
public:
	using ExtensionType = RTPExtensionType;
	using ExtensionManager = int;



	RtpPacket();
	explicit RtpPacket(const ExtensionManager* extensions);
	RtpPacket(const RtpPacket&);
	RtpPacket(const ExtensionManager* extensions, size_t capacity);
	~RtpPacket();

	RtpPacket& operator=(const RtpPacket&) = default;




private:
	ExtensionManager extensions_;
};

