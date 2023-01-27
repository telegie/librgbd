/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "yuv_frame.hpp"

#pragma warning(push)
#pragma warning(disable : 4244 26812)
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma warning(pop)

#include "png_utils.hpp"

namespace rgbd
{
// A helper function for YuvFrame::create(VpxImage) that converts a VpxImage
// into a vector.
vector<uint8_t> convert_channel_plane_to_bytes(const uint8_t* buffer,
                                               const int stride,
                                               const int width,
                                               const int height) noexcept
{
    vector<uint8_t> bytes(gsl::narrow<size_t>(width * height));
    for (int i{0}; i < height; ++i)
        memcpy(bytes.data() + gsl::narrow<int>(i * width),
               buffer + gsl::narrow<int>(i * stride),
               width);

    return bytes;
}

YuvFrame::YuvFrame(const int width,
                   const int height,
                   const uint8_t* y_channel,
                   const uint8_t* u_channel,
                   const uint8_t* v_channel)
    : width_{width}
    , height_{height}
    , y_channel_{}
    , u_channel_{}
    , v_channel_{}

{
    int y_size{width * height};
    int uv_size{y_size / 4};
    y_channel_.insert(y_channel_.end(), &y_channel[0], &y_channel[y_size]);
    u_channel_.insert(u_channel_.end(), &u_channel[0], &u_channel[uv_size]);
    v_channel_.insert(v_channel_.end(), &v_channel[0], &v_channel[uv_size]);
}

YuvFrame::YuvFrame(const int width,
                   const int height,
                   vector<uint8_t>&& y_channel,
                   vector<uint8_t>&& u_channel,
                   vector<uint8_t>&& v_channel) noexcept
    : y_channel_(std::move(y_channel))
    , u_channel_(std::move(u_channel))
    , v_channel_(std::move(v_channel))
    , width_{width}
    , height_{height}
{
    Expects(y_channel_.size() == (width * height));
    Expects(u_channel_.size() == (width * height / 4));
    Expects(v_channel_.size() == (width * height / 4));
}

YuvFrame::YuvFrame(AVFrameHandle& av_frame)
    : y_channel_()
    , u_channel_()
    , v_channel_()
    , width_{gsl::narrow<int>(av_frame->width)}
    , height_{gsl::narrow<int>(av_frame->height)}
{
    y_channel_ =
        convert_channel_plane_to_bytes(av_frame->data[0], av_frame->linesize[0], width_, height_);
    u_channel_ = convert_channel_plane_to_bytes(
        av_frame->data[1], av_frame->linesize[1], width_ / 2, height_ / 2);
    v_channel_ = convert_channel_plane_to_bytes(
        av_frame->data[2], av_frame->linesize[2], width_ / 2, height_ / 2);
}

YuvFrame YuvFrame::createFromAzureKinectYuy2Buffer(const uint8_t* buffer,
                                                   const int width,
                                                   const int height,
                                                   const int stride_bytes,
                                                   const int downsample)
{
    if (downsample == 1) {
        return createFromAzureKinectYuy2BufferOriginalSize(buffer, width, height, stride_bytes);
    } else if (downsample == 2) {
        return createFromAzureKinectYuy2BufferHalfSize(buffer, width, height, stride_bytes);
    } else {
        throw std::runtime_error(fmt::format(
            "Unsupported downsample ({}) found from createFromAzureKinectYuy2Buffer.", downsample));
    }
}

YuvFrame YuvFrame::getDownsampled(int downsampling_factor) const
{
    int downsampled_width{width_ / downsampling_factor};
    int downsampled_height{height_ / downsampling_factor};
    vector<uint8_t> downsampled_y_channel(static_cast<int64_t>(downsampled_width) *
                                          downsampled_height);

    int uv_width{width_ / 2};
    int downsampled_uv_width{downsampled_width / 2};
    int downsampled_uv_height{downsampled_height / 2};
    vector<uint8_t> downsampled_u_channel(static_cast<int64_t>(downsampled_uv_width) *
                                          downsampled_uv_height);
    vector<uint8_t> downsampled_v_channel(static_cast<int64_t>(downsampled_uv_width) *
                                          downsampled_uv_height);

    for (int row{0}; row < downsampled_height; ++row) {
        int downsampled_index{row * downsampled_width};
        int index{row * downsampling_factor * width_};
        for (int col{0}; col < downsampled_width; ++col) {
            downsampled_y_channel[downsampled_index] = y_channel_[index];
            ++downsampled_index;
            index += downsampling_factor;
        }
    }

    for (int row{0}; row < downsampled_uv_height; ++row) {
        int downsampled_index{row * downsampled_uv_width};
        int index{row * downsampling_factor * uv_width};
        for (int col{0}; col < downsampled_uv_width; ++col) {
            downsampled_u_channel[downsampled_index] = u_channel_[index];
            downsampled_v_channel[downsampled_index] = v_channel_[index];
            ++downsampled_index;
            index += downsampling_factor;
        }
    }

    return YuvFrame{downsampled_width,
                    downsampled_height,
                    std::move(downsampled_y_channel),
                    std::move(downsampled_u_channel),
                    std::move(downsampled_v_channel)};
}

YuvFrame YuvFrame::getMkvCoverSized() const
{
    // Pick the largest centered square area.
    int y_row_start{0};
    int y_row_end{height_};
    int y_col_start{0};
    int y_col_end{width_};
    if (width_ > height_) {
        y_col_start = width_ / 2 - height_ / 2;
        y_col_end = width_ / 2 + height_ / 2;
    } else {
        y_row_start = height_ / 2 - width_ / 2;
        y_row_end = height_ / 2 + width_ / 2;
    }

    constexpr int COVER_SIZE{600};
    std::vector<uint8_t> cover_y_channel(COVER_SIZE * COVER_SIZE, 0);
    std::vector<uint8_t> cover_u_channel(COVER_SIZE * COVER_SIZE / 4, 0);
    std::vector<uint8_t> cover_v_channel(COVER_SIZE * COVER_SIZE / 4, 0);

    for (int cover_row{0}; cover_row < COVER_SIZE; ++cover_row) {
        int y_row{y_row_start + cover_row * (y_row_end - y_row_start) / COVER_SIZE};
        for (int cover_col{0}; cover_col < COVER_SIZE; ++cover_col) {
            int y_col{y_col_start + cover_col * (y_col_end - y_col_start) / COVER_SIZE};
            cover_y_channel[cover_row * COVER_SIZE + cover_col] = y_channel_[y_row * width_ + y_col];
        }
    }

    for (int cover_row{0}; cover_row < COVER_SIZE; cover_row += 2) {
        int y_row{y_row_start + cover_row * (y_row_end - y_row_start) / COVER_SIZE};
        int uv_row{y_row / 2};
        for (int cover_col{0}; cover_col < COVER_SIZE; cover_col += 2) {
            int y_col{y_col_start + cover_col * (y_col_end - y_col_start) / COVER_SIZE};
            int uv_col{y_col / 2};
            cover_u_channel[(cover_row / 2) * (COVER_SIZE / 2) + (cover_col / 2)] =
                u_channel_[uv_row * (width_ / 2) + uv_col];
            cover_v_channel[(cover_row / 2) * (COVER_SIZE / 2) + (cover_col / 2)] =
                v_channel_[uv_row * (width_ / 2) + uv_col];
        }
    }

    return YuvFrame{COVER_SIZE,
                    COVER_SIZE,
                    std::move(cover_y_channel),
                    std::move(cover_u_channel),
                    std::move(cover_v_channel)};
}

Bytes YuvFrame::getPNGBytes() const
{
    std::vector<uint8_t> r_channel(width_ * height_, 0);
    std::vector<uint8_t> g_channel(width_ * height_, 0);
    std::vector<uint8_t> b_channel(width_ * height_, 0);
    std::vector<uint8_t> a_channel(width_ * height_, 255);

    for (int row{0}; row < height_; ++row) {
        for (int col{0}; col < width_; ++col) {
            uint8_t y{y_channel_[row * width_ + col]};
            uint8_t u{u_channel_[(row / 2) * (width_ / 2) + (col / 2)]};
            uint8_t v{v_channel_[(row / 2) * (width_ / 2) + (col / 2)]};

            // from https://en.wikipedia.org/wiki/YUV
            int r_tmp = y + ((351 * (v - 128)) >> 8);
            int g_tmp = y - ((179 * (v - 128) + 86 * (u - 128)) >> 8);
            int b_tmp = y + ((443 * (u - 128)) >> 8);
            uint8_t r = std::clamp(r_tmp, 0, 255);
            uint8_t g = std::clamp(g_tmp, 0, 255);
            uint8_t b = std::clamp(b_tmp, 0, 255);

            r_channel[row * width_ + col] = r;
            g_channel[row * width_ + col] = g;
            b_channel[row * width_ + col] = b;
        }
    }

    return PNGUtils::write(width_, height_, r_channel, g_channel, b_channel, a_channel);
}

YuvFrame YuvFrame::createFromAzureKinectYuy2BufferOriginalSize(const uint8_t* buffer,
                                                               const int width,
                                                               const int height,
                                                               const int stride_bytes) noexcept
{
    // Sizes assume Kinect runs in ColorImageFormat_Yuy2.
    vector<uint8_t> y_channel(gsl::narrow<size_t>(width * height));

    // Calculation of the U and V channels of the pixels.
    const int uv_width{width / 2};
    const int uv_height{height / 2};
    vector<uint8_t> u_channel(static_cast<int64_t>(uv_width) * uv_height);
    vector<uint8_t> v_channel(static_cast<int64_t>(uv_width) * uv_height);

    // Conversion of the Y channels of the pixels.
    int y_channel_index{0};
    for (int row{0}; row < height; ++row) {
        int buffer_index{row * stride_bytes};
        for (int col{0}; col < width; ++col) {
            y_channel[y_channel_index++] = buffer[buffer_index];
            buffer_index += 2;
        }
    }

    int uv_index{0};
    for (int row{0}; row < uv_height; ++row) {
        int buffer_index{row * stride_bytes * 2 + 1};
        for (int col{0}; col < uv_width; ++col) {
            u_channel[uv_index] = buffer[buffer_index];
            v_channel[uv_index] = buffer[buffer_index + 2];
            ++uv_index;
            buffer_index += 4;
        }
    }

    return YuvFrame(width, height, std::move(y_channel), std::move(u_channel), std::move(v_channel));
}

YuvFrame YuvFrame::createFromAzureKinectYuy2BufferHalfSize(const uint8_t* buffer,
                                                           const int width,
                                                           const int height,
                                                           const int stride_bytes) noexcept
{
    const int half_width{width / 2};
    const int half_height{height / 2};
    // Sizes assume Kinect runs in ColorImageFormat_Yuy2.
    vector<uint8_t> y_channel(gsl::narrow<size_t>(half_width * half_height));

    // Calculation of the U and V channels of the pixels.
    const int half_uv_width{half_width / 2};
    const int half_uv_height{half_height / 2};
    vector<uint8_t> u_channel(static_cast<int64_t>(half_uv_width) * half_uv_height);
    vector<uint8_t> v_channel(static_cast<int64_t>(half_uv_width) * half_uv_height);

    // Conversion of the Y channels of the pixels.
    int y_channel_index{0};
    for (int row{0}; row < half_height; ++row) {
        int buffer_index{row * stride_bytes * 2};
        for (int col{0}; col < half_width; ++col) {
            y_channel[y_channel_index++] = buffer[buffer_index];
            buffer_index += 4;
        }
    }

    int uv_index{0};
    for (int row{0}; row < half_uv_height; ++row) {
        int buffer_index{row * stride_bytes * 4 + 1};
        for (int col{0}; col < half_uv_width; ++col) {
            u_channel[uv_index] = buffer[buffer_index];
            v_channel[uv_index] = buffer[buffer_index + 2];
            ++uv_index;
            buffer_index += 8;
        }
    }

    return YuvFrame(
        half_width, half_height, std::move(y_channel), std::move(u_channel), std::move(v_channel));
}
} // namespace rgbd
