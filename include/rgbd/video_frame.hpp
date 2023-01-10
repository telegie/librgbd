#pragma once

#include "integer_frame.hpp"
#include "plane.hpp"
#include "yuv_frame.hpp"

namespace rgbd
{
class VideoFrame
{
public:
    VideoFrame(int64_t time_point_us,
               bool keyframe,
               unique_ptr<YuvFrame>&& yuv_frame,
               unique_ptr<Int32Frame>&& depth_frame,
               const optional<Plane>& floor) noexcept
        : time_point_us_{time_point_us}
        , keyframe_{keyframe}
        , yuv_frame_{std::move(yuv_frame)}
        , depth_frame_{std::move(depth_frame)}
        , floor_{floor}
    {
    }
    int64_t time_point_us() const noexcept
    {
        return time_point_us_;
    }
    bool keyframe() const noexcept
    {
        return keyframe_;
    }
    const unique_ptr<YuvFrame>& yuv_frame() const noexcept
    {
        return yuv_frame_;
    }
    unique_ptr<YuvFrame>& yuv_frame() noexcept
    {
        return yuv_frame_;
    }
    const unique_ptr<Int32Frame>& depth_frame() const noexcept
    {
        return depth_frame_;
    }
    unique_ptr<Int32Frame>& depth_frame() noexcept
    {
        return depth_frame_;
    }
    const optional<Plane>& floor() const noexcept
    {
        return floor_;
    }

private:
    int64_t time_point_us_;
    bool keyframe_;
    unique_ptr<YuvFrame> yuv_frame_;
    unique_ptr<Int32Frame> depth_frame_;
    vector<vector<float>> pcm_frames_;
    optional<Plane> floor_;
};
} // namespace rgbd
