#pragma once

#include "ffmpeg_utils.hpp"

namespace rgbd
{
class FFmpegAudioEncoder
{
public:
    FFmpegAudioEncoder();
    void encode(gsl::span<const float> pcm_samples, vector<AVPacketHandle>& packets);
    void flush(vector<AVPacketHandle>& packets);
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
