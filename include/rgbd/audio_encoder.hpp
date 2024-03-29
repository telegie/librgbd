#pragma once

#include "ffmpeg_utils.hpp"

namespace rgbd
{
struct AudioEncoderFrame
{
    vector<Bytes> packet_bytes_list;
};

class AudioEncoder
{
public:
    AudioEncoder();
    unique_ptr<AudioEncoderFrame> encode(span<const float> pcm_samples);
    unique_ptr<AudioEncoderFrame> flush();
    AVCodecContextHandle& codec_context()
    {
        return codec_context_;
    }
    int64_t next_pts()
    {
        return next_pts_;
    }

private:
    static void encodeAudioFrame(AVCodecContext* codec_context,
                                 AVFrame* frame,
                                 vector<Bytes>& packet_bytes);

private:
    AVCodecContextHandle codec_context_;
    AVFrameHandle frame_;
    int64_t next_pts_;
};
} // namespace rgbd
