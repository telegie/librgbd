#pragma once

#include "ffmpeg_utils.hpp"
#include "yuv_frame.hpp"

namespace tg
{
class FFmpegVideoDecoder
{
public:
    FFmpegVideoDecoder(ColorCodecType type);
    YuvFrame decode(gsl::span<const std::byte> vp8_frame);

private:
    AVCodecParserContextHandle codec_parser_context_;
    AVCodecContextHandle codec_context_;
    AVPacketHandle packet_;
};
} // namespace tg
