/*
Copyright (c) 2016, rpi-webrtc-streamer Lyu,KeunChang

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

#ifndef RASPI_ENCODER_IMPL_H_
#define RASPI_ENCODER_IMPL_H_

#include <memory>
#include <vector>

#include "system_wrappers/include/clock.h"
#include "rtc_base/platform_thread.h"
#include "modules/video_coding/include/video_codec_interface.h"
#include "media/engine/webrtcvideoencoderfactory.h"
#include "rtc_base/messagequeue.h"

#include "mmal_wrapper.h"
#include "raspi_quality_config.h"

#include "raspi_encoder.h"

namespace webrtc {

class RaspiEncoderImpl : public RaspiEncoder {
public:
    explicit RaspiEncoderImpl(const cricket::VideoCodec& codec);
    ~RaspiEncoderImpl() override;

    // number_of_cores, max_payload_size will be ignored.
    int32_t InitEncode(const VideoCodec* codec_settings,
                       int32_t number_of_cores,
                       size_t max_payload_size) override;
    int32_t Release() override;

    int32_t RegisterEncodeCompleteCallback(
        EncodedImageCallback* callback) override;
    int32_t SetRateAllocation(const BitrateAllocation& bitrate_allocation,
            uint32_t framerate) override;

    // The result of encoding - an EncodedImage and RTPFragmentationHeader - are
    // passed to the encode complete callback.
    int32_t Encode(const VideoFrame& frame,
                   const CodecSpecificInfo* codec_specific_info,
                   const std::vector<FrameType>* frame_types) override;

    const char* ImplementationName() const override;

    VideoEncoder::ScalingSettings GetScalingSettings() const override;

    // Unsupported / Do nothing.
    int32_t SetChannelParameters(uint32_t packet_loss, int64_t rtt) override;

private:
    bool IsInitialized() const;

    static bool DrainThread(void*);
    bool drainStarted_;
    bool DrainProcess();

    // Reports statistics with histograms.
    void ReportInit();
    void ReportError();

    // Change the value to enable when the Encode function is called.
    // Only when this flag is true will actually pass the Encoded Frame
    // to the native stack.
    bool start_encoding_;

    MMALEncoderWrapper *mmal_encoder_;

    bool has_reported_init_;
    bool has_reported_error_;

    //
    // Encoded frame process thread
    std::unique_ptr<rtc::PlatformThread> drainThread_;

    EncodedImageCallback* encoded_image_callback_;
    EncodedImage encoded_image_;

    // H264 bitstream parser, used to extract QP from encoded bitstreams.
    // webrtc::H264BitstreamParser h264_bitstream_parser_;

    Clock* const clock_;
    const int64_t delta_ntp_internal_ms_;
    int64_t base_internal_ms_;
    int64_t last_keyframe_request_;

    VideoCodecMode mode_;
    size_t max_payload_size_;
    int key_frame_interval_;

    // H.264 specifc parameters
    bool frame_dropping_on_;
    H264PacketizationMode packetization_mode_;

    // Quality Config
    QualityConfig quality_config_;
    int initial_delay_;
};

}  // namespace webrtc

#endif  // RASPI_ENCODER_IMPL_H_
