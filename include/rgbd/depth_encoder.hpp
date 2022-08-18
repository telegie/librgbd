#pragma once

#include "constants.hpp"

namespace rgbd
{
class DepthEncoder
{
public:
    virtual ~DepthEncoder() {}
    virtual DepthCodecType getCodecType() noexcept = 0;
    virtual Bytes encode(gsl::span<const int32_t> depth_values, bool keyframe) noexcept = 0;
};
}
