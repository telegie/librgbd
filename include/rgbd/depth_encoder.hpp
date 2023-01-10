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
private:
    DepthEncoder(unique_ptr<DepthEncoderImpl>&& impl);

public:
    static unique_ptr<DepthEncoder> createRVLEncoder(int width, int height);
    static unique_ptr<DepthEncoder>
    createTDC1Encoder(int width, int height, int depth_diff_multiplier);
    DepthCodecType getCodecType() noexcept;
    Bytes encode(const int32_t* depth_values, bool keyframe) noexcept;

private:
    unique_ptr<DepthEncoderImpl> impl_;
};
} // namespace rgbd
