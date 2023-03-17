#include "rgbd/ffmpeg_utils.hpp"

#pragma warning(push)
#pragma warning(disable : 4244 26812)
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
}
#pragma warning(pop)

namespace rgbd
{
AVCodec* find_decoder_avcodec(ColorCodecType color_codec_type)
{
    if (color_codec_type != ColorCodecType::VP8) {
        spdlog::error("Invalid ColorCodecType");
        throw std::runtime_error("Invalid ColorCodecType");
    }

    auto codec{avcodec_find_decoder(AV_CODEC_ID_VP8)};
    if (!codec) {
        spdlog::error("avcodec_find_decoder failed");
        throw std::runtime_error("avcodec_find_decoder failed.");
    }

    return codec;
}

AVCodec* find_encoder_avcodec(ColorCodecType color_codec_type)
{
    if (color_codec_type != ColorCodecType::VP8) {
        spdlog::error("Invalid ColorCodecType");
        throw std::runtime_error("Invalid ColorCodecType");
    }


    auto codec{avcodec_find_encoder(AV_CODEC_ID_VP8)};
    if (!codec) {
        spdlog::error("avcodec_find_encoder failed");
        throw std::runtime_error("avcodec_find_encoder failed.");
    }

    return codec;
}

AVCodecContextHandle::AVCodecContextHandle(const AVCodec* codec)
    : unique_ptr_{avcodec_alloc_context3(codec),
                  [](AVCodecContext* ptr) { avcodec_free_context(&ptr); }}
{
    if (!unique_ptr_) {
        spdlog::error("Error from AVCodecContextHandle::AVCodecContextHandle: {}", (size_t)unique_ptr_.get());
        throw std::runtime_error("Error from AVCodecContextHandle::AVCodecContextHandle");
    }
}

AVFrameHandle::AVFrameHandle()
    : shared_ptr_{av_frame_alloc(), [](AVFrame* ptr) { av_frame_free(&ptr); }}
{
    if (!shared_ptr_) {
        spdlog::error("Error from AVFrameHandle::AVFrameHandle");
        throw std::runtime_error("Error from AVFrameHandle::AVFrameHandle");
    }
}

AVPacketHandle::AVPacketHandle()
    : shared_ptr_{av_packet_alloc(), [](AVPacket* ptr) { av_packet_free(&ptr); }}
{
    if (!shared_ptr_) {
        spdlog::error("Error from AVPacketHandle::AVPacketHandle");
        throw std::runtime_error("Error from AVPacketHandle::AVPacketHandle");
    }
}

Bytes AVPacketHandle::getDataBytes()
{
    Bytes data_bytes;
    for (size_t i{0}; i < shared_ptr_->size; ++i)
        data_bytes.push_back(shared_ptr_->data[i]);
    return data_bytes;
}

AVCodecParserContextHandle::AVCodecParserContextHandle(int codec_id)
    : unique_ptr_{av_parser_init(codec_id),
                    [](AVCodecParserContext* ptr) { av_parser_close(ptr); }}
{
    if (!unique_ptr_.get()) {
        spdlog::error("Error from AVCodecParserContextHandle::AVCodecParserContextHandle");
        throw std::runtime_error(
                "Error from AVCodecParserContextHandle::AVCodecParserContextHandle");
    }
}
}
