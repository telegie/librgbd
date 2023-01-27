/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "ffmpeg_utils.hpp"

namespace rgbd
{
// An class that contains color pixels in the YUV420 format which Vp8Encoder and
// Vp8Decoder like. Data of this class is not supposed to be copy since it is
// computationally expensive.
class YuvFrame
{
public:
    YuvFrame(const int width,
             const int height,
             const uint8_t* y_channel,
             const uint8_t* u_channel,
             const uint8_t* v_channel);
    YuvFrame(const int width,
             const int height,
             vector<uint8_t>&& y_channel,
             vector<uint8_t>&& u_channel,
             vector<uint8_t>&& v_channel) noexcept;
    YuvFrame(AVFrameHandle& av_frame);
    static YuvFrame createFromAzureKinectYuy2Buffer(
        const uint8_t* buffer, int width, int height, int stride_bytes, int downsample);
    YuvFrame getDownsampled(int downsampling_factor) const;
    YuvFrame getMkvCoverSized() const;
    Bytes getPNGBytes() const;
    int width() const noexcept
    {
        return width_;
    }
    int height() const noexcept
    {
        return height_;
    }
    const vector<uint8_t>& y_channel() const noexcept
    {
        return y_channel_;
    }
    vector<uint8_t>& y_channel() noexcept
    {
        return y_channel_;
    }
    const vector<uint8_t>& u_channel() const noexcept
    {
        return u_channel_;
    }
    vector<uint8_t>& u_channel() noexcept
    {
        return u_channel_;
    }
    const vector<uint8_t>& v_channel() const noexcept
    {
        return v_channel_;
    }
    vector<uint8_t>& v_channel() noexcept
    {
        return v_channel_;
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
    int width_;
    int height_;
    vector<uint8_t> y_channel_;
    vector<uint8_t> u_channel_;
    vector<uint8_t> v_channel_;
};
} // namespace rgbd
