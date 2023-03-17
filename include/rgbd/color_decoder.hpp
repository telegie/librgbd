#pragma once

#include "yuv_frame.hpp"

namespace rgbd
{
class ColorDecoder
{
public:
    ColorDecoder(ColorCodecType type);
    unique_ptr<YuvFrame> decode(span<const uint8_t> vp8_frame);

private:
    AVCodecParserContextHandle codec_parser_context_;
    AVCodecContextHandle codec_context_;
    AVPacketHandle packet_;
};
} // namespace rgbd
