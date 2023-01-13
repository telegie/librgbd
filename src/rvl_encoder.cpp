#include "rvl_encoder.hpp"

#include "byte_utils.hpp"
#include "rvl.hpp"

namespace rgbd
{
RVLEncoder::RVLEncoder(int width, int height) noexcept
    : width_{width}
    , height_{height}
{
}

DepthCodecType RVLEncoder::getCodecType() noexcept
{
    return DepthCodecType::RVL;
}

Bytes RVLEncoder::encode(const int32_t* depth_values, bool keyframe) noexcept
{
    Bytes bytes;
    append_bytes(bytes, convert_to_bytes(width_));
    append_bytes(bytes, convert_to_bytes(height_));
    size_t size{static_cast<size_t>(width_ * height_)};
    append_bytes(bytes, rvl::compress(span<const int32_t>{depth_values, size}));

    return bytes;
}
}