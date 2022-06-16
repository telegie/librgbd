#pragma once

#include "constants.hpp"

namespace rgbd
{
class DepthEncoder
{
public:
    virtual ~DepthEncoder() {}
    virtual Bytes encode(gsl::span<const int16_t> depth_values, bool keyframe) noexcept = 0;
};
}
