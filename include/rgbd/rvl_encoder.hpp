#pragma once

#include "depth_encoder.hpp"

namespace rgbd
{
class RVLEncoder : public DepthEncoderImpl
{
public:
    RVLEncoder(int width, int height) noexcept;
    DepthCodecType getCodecType() noexcept;
    Bytes encode(gsl::span<const int32_t> depth_values, bool keyframe) noexcept;

private:
    const int width_;
    const int height_;
};
} // namespace rgbd
