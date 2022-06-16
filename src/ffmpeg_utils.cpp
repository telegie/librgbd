#include "rgbd/ffmpeg_utils.hpp"

namespace rgbd
{
AVCodec* find_decoder_avcodec(ColorCodecType color_codec_type)
{
    if (color_codec_type != ColorCodecType::VP8)
        throw std::runtime_error("Invalid ColorCodecType");

    auto codec{avcodec_find_decoder(AV_CODEC_ID_VP8)};
    if (!codec)
        throw std::runtime_error("avcodec_find_decoder failed.");

    return codec;
}

AVCodec* find_encoder_avcodec(ColorCodecType color_codec_type)
{
    if (color_codec_type != ColorCodecType::VP8)
        throw std::runtime_error("Invalid ColorCodecType");

    auto codec{avcodec_find_encoder(AV_CODEC_ID_VP8)};
    if (!codec)
        throw std::runtime_error("avcodec_find_decoder failed.");

    return codec;
}
}
