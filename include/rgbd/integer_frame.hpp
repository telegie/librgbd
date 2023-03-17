#pragma once

#include "constants.hpp"

namespace rgbd
{
template <class T> class IntegerFrame
{
public:
    IntegerFrame(int width, int height, const T* values)
        : width_{width}
        , height_{height}
        , values_(values, values + (static_cast<int64_t>(width) * height))
    {
    }
    IntegerFrame(int width, int height, const vector<T>& values)
        : width_{width}
        , height_{height}
        , values_(values)
    {
        Expects(values.size() == (width * height));
    }
    unique_ptr<IntegerFrame> getDownsampled(int downsampling_factor) const
    {
        int downsampled_width{width_ / downsampling_factor};
        int downsampled_height{height_ / downsampling_factor};
        vector<T> downsampled_values(static_cast<int64_t>(downsampled_width) * downsampled_height);
        for (int row{0}; row < downsampled_height; ++row) {
            int downsampled_index{row * downsampled_width};
            int index{row * downsampling_factor * width_};
            for (int col{0}; col < downsampled_width; ++col) {
                downsampled_values[downsampled_index] = values_[index];
                ++downsampled_index;
                index += downsampling_factor;
            }
        }
        return std::make_unique<IntegerFrame<T>>(downsampled_width, downsampled_height, downsampled_values);
    }
    int width() const noexcept
    {
        return width_;
    }
    int height() const noexcept
    {
        return height_;
    }
    vector<T>& values() noexcept
    {
        return values_;
    }
    const vector<T>& values() const noexcept
    {
        return values_;
    }

private:
    int width_;
    int height_;
    vector<T> values_;
};

using Int32Frame = IntegerFrame<int32_t>;
using UInt8Frame = IntegerFrame<uint8_t>;
} // namespace rgbd
