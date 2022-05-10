#include "RtpPacket.h"
#include <cstdint>


namespace {
	constexpr size_t kFixedHeaderSize = 12;
	constexpr uint8_t kRtpVersion = 2;
	constexpr uint16_t kOneByteExtensionProfileId = 0xBEDE;
	constexpr uint16_t kTwoByteExtensionProfileId = 0x1000;
	constexpr size_t kOneByteExtensionHeaderLength = 1;
	constexpr size_t kTwoByteExtensionHeaderLength = 2;
	constexpr size_t kDefaultPacketSize = 1500;
}  // namespace


//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |V=2|P|X|  CC   |M|     PT      |       sequence number         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                           timestamp                           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |           synchronization source (SSRC) identifier            |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |            Contributing source (CSRC) identifiers             |
// |                             ....                              |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |  header eXtension profile id  |       length in 32bits        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                          Extensions                           |
// |                             ....                              |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |                           Payload                             |
// |             ....              :  padding...                   |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |               padding         | Padding size  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RtpPacket::RtpPacket():RtpPacket(nullptr, kDefaultPacketSize) {}

RtpPacket::RtpPacket(const ExtensionManager * extensions)
{
}


RtpPacket::RtpPacket(const RtpPacket &)
{
}

RtpPacket::RtpPacket(const ExtensionManager * extensions, size_t capacity)
	:extensions_(extensions ? *extensions : ExtensionManager())
{


}

RtpPacket::~RtpPacket()
{
}
