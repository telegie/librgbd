#pragma once

#include "yuv_frame.hpp"

namespace rgbd
{
class ColorEncoder
{
public:
    ColorEncoder(ColorCodecType type, int width, int height);
    Bytes encode(const YuvFrame& yuv_image, bool keyframe);
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
} // namespace rgbd
