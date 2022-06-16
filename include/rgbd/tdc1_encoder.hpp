#pragma once

#include "depth_encoder.hpp"
#include "rvl.hpp"

namespace rgbd
{
class TDC1Encoder : public DepthEncoder
{
public:
    TDC1Encoder(int width, int height, int diff_multiplier) noexcept;
    Bytes encode(gsl::span<const int16_t> depth_values, bool keyframe) noexcept;

private:
    const int width_;
    const int height_;
    const int diff_multiplier_;
    vector<int16_t> previous_depth_values_;
};
} // namespace tg
