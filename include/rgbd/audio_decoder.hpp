#pragma once

#include "ffmpeg_utils.hpp"

namespace rgbd
{
class AudioDecoder
{
public:
    AudioDecoder();
    vector<float> decode(span<const byte> opus_frame);

private:
    AVCodecParserContextHandle codec_parser_context_;
    AVCodecContextHandle codec_context_;
    AVPacketHandle packet_;
};
} // namespace rgbd
