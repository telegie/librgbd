/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include <gsl/gsl>
#include <spdlog/spdlog.h>
#include "ffmpeg_utils.hpp"

namespace tg
{
// An class that contains color pixels in the YUV420 format which Vp8Encoder and
// Vp8Decoder like. Data of this class is not supposed to be copy since it is
// computationally expensive.
class YuvFrame
{
public:
    YuvFrame(std::vector<uint8_t>&& y_channel,
             std::vector<uint8_t>&& u_channel,
             std::vector<uint8_t>&& v_channel,
             const int width,
             const int height) noexcept;
    YuvFrame(AVFrameHandle& av_frame);
    YuvFrame(const YuvFrame& other) noexcept;
    YuvFrame(YuvFrame&& other) noexcept;
    YuvFrame& operator=(const YuvFrame& other) = delete;
    YuvFrame& operator=(YuvFrame&& other) = delete;
    static YuvFrame createFromAzureKinectYuy2Buffer(
        const uint8_t* buffer, int width, int height, int stride_bytes, int downsample);
    YuvFrame getDownsampled(int downsampling_factor) const;
    const std::vector<uint8_t>& y_channel() const noexcept
    {
        return y_channel_;
    }
    std::vector<uint8_t>& y_channel() noexcept
    {
        return y_channel_;
    }
    const std::vector<uint8_t>& u_channel() const noexcept
    {
        return u_channel_;
    }
    std::vector<uint8_t>& u_channel() noexcept
    {
        return u_channel_;
    }
    const std::vector<uint8_t>& v_channel() const noexcept
    {
        return v_channel_;
    }
    std::vector<uint8_t>& v_channel() noexcept
    {
        return v_channel_;
    }
    int width() const noexcept
    {
        return width_;
    }
    int height() const noexcept
    {
        return height_;
    }

private:
    static YuvFrame createFromAzureKinectYuy2BufferOriginalSize(const uint8_t* buffer,
                                                                int width,
                                                                int height,
                                                                int stride) noexcept;
    static YuvFrame createFromAzureKinectYuy2BufferHalfSize(const uint8_t* buffer,
                                                            int width,
                                                            int height,
                                                            int stride) noexcept;

private:
    std::vector<uint8_t> y_channel_;
    std::vector<uint8_t> u_channel_;
    std::vector<uint8_t> v_channel_;
    int width_;
    int height_;
};
} // namespace tg
