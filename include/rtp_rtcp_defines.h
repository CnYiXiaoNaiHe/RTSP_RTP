#pragma once

enum RTPExtensionType : int {
	kRtpExtensionNone,
	kRtpExtensionTransmissionTimeOffset,
	kRtpExtensionAudioLevel,
	kRtpExtensionAbsoluteSendTime,
	kRtpExtensionVideoRotation,
	kRtpExtensionTransportSequenceNumber,
	kRtpExtensionTransportSequenceNumber02,
	kRtpExtensionPlayoutDelay,
	kRtpExtensionVideoContentType,
	kRtpExtensionVideoTiming,
	kRtpExtensionFrameMarking,
	kRtpExtensionRtpStreamId,
	kRtpExtensionRepairedRtpStreamId,
	kRtpExtensionMid,
	kRtpExtensionGenericFrameDescriptor00,
	kRtpExtensionGenericFrameDescriptor = kRtpExtensionGenericFrameDescriptor00,
	kRtpExtensionGenericFrameDescriptor01,
	kRtpExtensionColorSpace,
	kRtpExtensionNumberOfExtensions  // Must be the last entity in the enum.
};
