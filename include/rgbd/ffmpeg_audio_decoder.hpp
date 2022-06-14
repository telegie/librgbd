#pragma once

#include "ffmpeg_utils.hpp"

namespace tg
{
class FFmpegAudioDecoder
{
public:
    FFmpegAudioDecoder();
    vector<float> decode(gsl::span<const std::byte> opus_frame);

private:
    AVCodecParserContextHandle codec_parser_context_;
    AVCodecContextHandle codec_context_;
    AVPacketHandle packet_;
};
} // namespace tg
