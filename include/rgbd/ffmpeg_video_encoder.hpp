#pragma once

#include "ffmpeg_utils.hpp"
#include "yuv_frame.hpp"

namespace rgbd
{
class FFmpegVideoEncoder
{
public:
    FFmpegVideoEncoder(
        ColorCodecType type, int width, int height, int target_bitrate, int framerate);
    AVPacketHandle encode(const YuvFrame& yuv_image, bool keyframe);
    AVPacketHandle encode(gsl::span<const uint8_t> y_channel,
                          gsl::span<const uint8_t> u_channel,
                          gsl::span<const uint8_t> v_channel,
                          const bool keyframe);
    AVCodecContextHandle& codec_context()
    {
        return codec_context_;
    }
    int64_t next_pts()
    {
        return next_pts_;
    }

private:
    static vector<AVPacketHandle> encodeVideoFrame(AVCodecContext* codec_context, AVFrame* frame);

private:
    AVCodecContextHandle codec_context_;
    AVFrameHandle frame_;
    int64_t next_pts_;
};
} // namespace tg
