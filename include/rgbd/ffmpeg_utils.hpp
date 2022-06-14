#pragma once

#pragma warning(push)
#pragma warning(disable : 4244 26812)
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
}
#pragma warning(push)

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

namespace tg
{
enum class ColorCodecType : int32_t
{
    VP8 = 0
};

enum class DepthCodecType : int32_t
{
    RVL = 0,
    TDC1 = 1
};

AVCodec* find_decoder_avcodec(ColorCodecType color_codec_type);
AVCodec* find_encoder_avcodec(ColorCodecType color_codec_type);

class AVCodecContextHandle
{
public:
    AVCodecContextHandle(const AVCodec* codec)
        : unique_ptr_{avcodec_alloc_context3(codec),
                      [](AVCodecContext* ptr) { avcodec_free_context(&ptr); }}
    {
        if (!unique_ptr_.get())
            throw std::runtime_error("Error from AVCodecContextHandle::AVCodecContextHandle");
    }
    AVCodecContext* get() const noexcept
    {
        return unique_ptr_.get();
    }
    AVCodecContext* operator->() const noexcept
    {
        return unique_ptr_.get();
    }

private:
    std::unique_ptr<AVCodecContext, std::function<void(AVCodecContext*)>> unique_ptr_;
};

class AVFrameHandle
{
public:
    AVFrameHandle()
        : shared_ptr_{av_frame_alloc(), [](AVFrame* ptr) { av_frame_free(&ptr); }}
    {
        if (!shared_ptr_.get())
            throw std::runtime_error("Error from AVFrameHandle::AVFrameHandle");
    }
    AVFrame* get() const noexcept
    {
        return shared_ptr_.get();
    }
    AVFrame* operator->() const noexcept
    {
        return shared_ptr_.get();
    }

private:
    std::shared_ptr<AVFrame> shared_ptr_;
};

class AVPacketHandle
{
public:
    AVPacketHandle()
        : shared_ptr_{av_packet_alloc(), [](AVPacket* ptr) { av_packet_free(&ptr); }}
    {
        if (!shared_ptr_.get())
            throw std::runtime_error("Error from AVPacketHandle::AVPacketHandle");
    }
    std::vector<std::byte> getDataBytes()
    {
        std::vector<std::byte> data_bytes;
        for (size_t i{0}; i < shared_ptr_->size; ++i)
            data_bytes.push_back(static_cast<std::byte>(shared_ptr_->data[i]));
        return data_bytes;
    }
    AVPacket* get() const noexcept
    {
        return shared_ptr_.get();
    }
    AVPacket* operator->() const noexcept
    {
        return shared_ptr_.get();
    }

private:
    std::shared_ptr<AVPacket> shared_ptr_;
};

class AVCodecParserContextHandle
{
public:
    AVCodecParserContextHandle(int codec_id)
        : unique_ptr_{av_parser_init(codec_id),
                      [](AVCodecParserContext* ptr) { av_parser_close(ptr); }}
    {
        if (!unique_ptr_.get())
            throw std::runtime_error(
                "Error from AVCodecParserContextHandle::AVCodecParserContextHandle");
    }
    AVCodecParserContext* get() const noexcept
    {
        return unique_ptr_.get();
    }
    AVCodecParserContext* operator->() const noexcept
    {
        return unique_ptr_.get();
    }

private:
    std::unique_ptr<AVCodecParserContext, std::function<void(AVCodecParserContext*)>> unique_ptr_;
};
} // namespace tg
