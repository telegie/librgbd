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

Bytes RVLEncoder::encode(gsl::span<const int32_t> depth_values, bool keyframe) noexcept
{
    Bytes bytes;
    append_bytes(bytes, convert_to_bytes(width_));
    append_bytes(bytes, convert_to_bytes(height_));
    append_bytes(bytes, rvl::compress(depth_values));

    return bytes;
}
}