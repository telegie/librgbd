#pragma once

#include "rvl.hpp"

namespace rgbd
{
class DepthEncoderImpl
{
public:
    virtual ~DepthEncoderImpl() {}
    virtual DepthCodecType getCodecType() noexcept = 0;
    virtual Bytes encode(const int32_t* depth_values, bool keyframe) noexcept = 0;
};

class DepthEncoder
{
public:
    DepthEncoder(DepthCodecType type, int width, int height);
    DepthCodecType getCodecType() noexcept;
    Bytes encode(const int32_t* depth_values, bool keyframe) noexcept;

private:
    unique_ptr<DepthEncoderImpl> impl_;
};
} // namespace rgbd
