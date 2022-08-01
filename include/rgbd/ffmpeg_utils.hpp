#pragma once


#include "constants.hpp"

class AVCodec;
class AVCodecContext;
class AVCodecParserContext;
class AVFrame;
class AVPacket;

namespace rgbd
{
AVCodec* find_decoder_avcodec(ColorCodecType color_codec_type);
AVCodec* find_encoder_avcodec(ColorCodecType color_codec_type);

class AVCodecContextHandle
{
public:
    AVCodecContextHandle(const AVCodec* codec);
    AVCodecContext* get() const noexcept
    {
        return unique_ptr_.get();
    }
    AVCodecContext* operator->() const noexcept
    {
        return unique_ptr_.get();
    }

private:
    unique_ptr<AVCodecContext, std::function<void(AVCodecContext*)>> unique_ptr_;
};

class AVFrameHandle
{
public:
    AVFrameHandle();
    AVFrame* get() const noexcept
    {
        return shared_ptr_.get();
    }
    AVFrame* operator->() const noexcept
    {
        return shared_ptr_.get();
    }

private:
    shared_ptr<AVFrame> shared_ptr_;
};

class AVPacketHandle
{
public:
    AVPacketHandle();
    Bytes getDataBytes();
    AVPacket* get() const noexcept
    {
        return shared_ptr_.get();
    }
    AVPacket* operator->() const noexcept
    {
        return shared_ptr_.get();
    }

private:
    shared_ptr<AVPacket> shared_ptr_;
};

class AVCodecParserContextHandle
{
public:
    AVCodecParserContextHandle(int codec_id);
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
