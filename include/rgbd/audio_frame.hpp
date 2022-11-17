#pragma once

#include "constants.hpp"

namespace rgbd
{
class AudioFrame
{
public:
    AudioFrame(int64_t time_point_us, const vector<float>& pcm_samples) noexcept
        : time_point_us_{time_point_us}
        , pcm_samples_{pcm_samples}
    {
    }
    int64_t time_point_us() const noexcept
    {
        return time_point_us_;
    }
    const vector<float>& pcm_samples() const noexcept
    {
        return pcm_samples_;
    }

private:
    int64_t time_point_us_;
    vector<float> pcm_samples_;
};
} // namespace rgbd
