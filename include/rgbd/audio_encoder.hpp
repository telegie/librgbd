#pragma once

#include "ffmpeg_utils.hpp"

namespace rgbd
{
struct AudioEncoderFrame
{
    vector<AVPacketHandle> packets;

    // Here to make sure constructor of packets is called.
    AudioEncoderFrame()
        : packets()
    {
    }
};

class AudioEncoder
{
public:
    AudioEncoder();
    unique_ptr<AudioEncoderFrame> encode(gsl::span<const float> pcm_samples);
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
                                 vector<AVPacketHandle>& packets);

private:
    AVCodecContextHandle codec_context_;
    AVFrameHandle frame_;
    int64_t next_pts_;
};
} // namespace tg
