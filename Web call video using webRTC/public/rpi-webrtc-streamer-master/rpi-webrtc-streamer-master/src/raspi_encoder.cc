/*
Copyright (c) 2017, rpi-webrtc-streamer Lyu,KeunChang

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <limits>
#include <string>

#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/ptr_util.h"

#include "common_types.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "common_video/h264/h264_common.h"
#include "common_video/h264/profile_level_id.h"

#include "media/base/codec.h"

#include "raspi_encoder.h"
#include "raspi_encoder_impl.h"

namespace webrtc {

std::unique_ptr<RaspiEncoder> RaspiEncoder::Create(
    const cricket::VideoCodec& codec) {
  RTC_LOG(LS_INFO) << "Creating H264EncoderImpl.";
  return rtc::MakeUnique<RaspiEncoderImpl>(codec);
}

bool RaspiEncoder::IsSupported() {
  return true;
}

RaspiVideoEncoderFactory* RaspiVideoEncoderFactory::CreateVideoEncoderFactory() {
  RTC_LOG(LS_INFO) << "Creating RaspiVideoEncoderFactory.";
  return new RaspiVideoEncoderFactory;
}

// borrowed from src/modules/video_coding/codecs/h264/h264.cc
static bool IsSameFormat(const webrtc::SdpVideoFormat& format1,
                  const webrtc::SdpVideoFormat& format2) {
  // If different names (case insensitive), then not same formats.
  if (!cricket::CodecNamesEq(format1.name, format2.name))
    return false;
  // For every format besides H264, comparing names is enough.
  if (!cricket::CodecNamesEq(format1.name.c_str(), cricket::kH264CodecName))
    return true;
  // Compare H264 profiles.
  const rtc::Optional<webrtc::H264::ProfileLevelId> profile_level_id =
      webrtc::H264::ParseSdpProfileLevelId(format1.parameters);
  const rtc::Optional<webrtc::H264::ProfileLevelId> other_profile_level_id =
      webrtc::H264::ParseSdpProfileLevelId(format2.parameters);
  // Compare H264 profiles, but not levels.
  return profile_level_id && other_profile_level_id &&
         profile_level_id->profile == other_profile_level_id->profile;
}

static SdpVideoFormat CreateH264Format(H264::Profile profile, H264::Level level) {
    const rtc::Optional<std::string> profile_string =
        H264::ProfileLevelIdToString(H264::ProfileLevelId(profile, level));
    RTC_CHECK(profile_string);
    return SdpVideoFormat(cricket::kH264CodecName,
            {{cricket::kH264FmtpProfileLevelId, *profile_string},
            {cricket::kH264FmtpLevelAsymmetryAllowed, "1"},
            {cricket::kH264FmtpPacketizationMode, "1"}});
}

static std::vector<SdpVideoFormat> SupportedH264Codecs() {
    return {CreateH264Format(H264::kProfileBaseline, H264::kLevel3_1),
        CreateH264Format(H264::kProfileConstrainedBaseline, H264::kLevel3_1)};
}

///////////////////////////////////////////////////////////////////////////////
//
// Raspi Video Encoder Factory
//
///////////////////////////////////////////////////////////////////////////////
RaspiVideoEncoderFactory::RaspiVideoEncoderFactory() {
    RTC_LOG(INFO) << "Raspi H.264 Video encoder factory.";
    supported_formats_ = SupportedH264Codecs();
}

webrtc::VideoEncoderFactory::CodecInfo RaspiVideoEncoderFactory::QueryVideoEncoder(
        const webrtc::SdpVideoFormat& format) const {

    for (const webrtc::SdpVideoFormat& supported_format : supported_formats_) {
        if (IsSameFormat(format, supported_format)) {
            RTC_LOG(INFO) << "Codec " << format.name
                << " found in RaspiVideoEncoderFactory supported format";
            // RaspiVideoEncoder only supports InternalSource H.264 Video Codec
            VideoEncoderFactory::CodecInfo info;
            info.has_internal_source = true;
            info.is_hardware_accelerated = true;
            return info;
        }
    }

    RTC_LOG(LS_ERROR) << "Codec " << format.name
        << " does not support in RaspiVideoEncoderFactory";
    VideoEncoderFactory::CodecInfo info;
    info.has_internal_source = false;
    info.is_hardware_accelerated = false;
    return info;
}

std::unique_ptr<webrtc::VideoEncoder> RaspiVideoEncoderFactory::CreateVideoEncoder(
        const webrtc::SdpVideoFormat& format) {
    const cricket::VideoCodec codec(format);

    // Try creating external encoder.
    std::unique_ptr<webrtc::VideoEncoder> video_encoder;

    for (const webrtc::SdpVideoFormat& supported_format : supported_formats_) {
        if (IsSameFormat(format, supported_format)) {
            RTC_LOG(INFO) << "Raspi " << format.name << " video encoder created";
            video_encoder = RaspiEncoder::Create(codec);
            return move(video_encoder);
        }
    }
    return move(video_encoder);
}

RaspiVideoEncoderFactory::~RaspiVideoEncoderFactory() {
    RTC_LOG(INFO) << "Raspi Video encoder factory destroy";
}



}  // namespace webrtc
