/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#ifndef RASPI_DECODER_DUMMY_H_
#define RASPI_DECODER_DUMMY_H_

#include <memory>

#include "api/video/video_frame.h"
#include "api/video_codecs/video_codec.h"
#include "common_video/include/video_frame.h"

namespace webrtc {

class RaspiDecoderDummy : public RaspiDecoder {
public:
    RaspiDecoderDummy();
    ~RaspiDecoderDummy() override;

    int32_t InitDecode(const VideoCodec* codec_settings,
            int32_t number_of_cores) override;
    int32_t Release() override;

    int32_t RegisterDecodeCompleteCallback(
            DecodedImageCallback* callback) override;

    int32_t Decode(const EncodedImage& input_image,
            bool /*missing_frames*/,
            const CodecSpecificInfo* codec_specific_info = nullptr,
            int64_t render_time_ms = -1) override;

    const char* ImplementationName() const override;

private:
    bool IsInitialized() const;

    VideoCodec decoder_config_;
    DecodedImageCallback* decoded_image_callback_;
    bool decoder_initialized_;
};

}  // namespace webrtc

#endif  // RASPI_DECODER_DUMMY_H_
