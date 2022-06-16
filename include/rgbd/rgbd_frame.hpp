#pragma once

#include <rgbd/yuv_frame.hpp>
#include <rgbd/integer_frame.hpp>
#include "plane.hpp"

namespace tg
{
class RGBDFrame
{
public:
    RGBDFrame(int64_t time_point_us,
              YuvFrame&& yuv_frame,
              Int16Frame&& depth_frame,
              optional<UInt8Frame>&& depth_confidence_frame,
              const Plane& floor) noexcept
        : time_point_us_{time_point_us}
        , yuv_frame_{std::move(yuv_frame)}
        , depth_frame_{std::move(depth_frame)}
        , depth_confidence_frame_{std::move(depth_confidence_frame)}
        , floor_{floor}
    {
    }
    int64_t time_point_us() const noexcept
    {
        return time_point_us_;
    }
    const YuvFrame& yuv_frame() const noexcept
    {
        return yuv_frame_;
    }
    YuvFrame& yuv_frame() noexcept
    {
        return yuv_frame_;
    }
    const Int16Frame& depth_frame() const noexcept
    {
        return depth_frame_;
    }
    Int16Frame& depth_frame() noexcept
    {
        return depth_frame_;
    }
    const optional<UInt8Frame>& depth_confidence_frame() const noexcept
    {
        return depth_confidence_frame_;
    }
    const Plane& floor() const noexcept
    {
        return floor_;
    }

private:
    int64_t time_point_us_;
    YuvFrame yuv_frame_;
    Int16Frame depth_frame_;
    optional<UInt8Frame> depth_confidence_frame_;
    vector<vector<float>> pcm_frames_;
    Plane floor_;
};
} // namespace tg
