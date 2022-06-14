#include "int16_frame.hpp"

namespace tg
{
Int16Frame::Int16Frame(const int16_t* buffer, int width, int height)
    : width_{width}
    , height_{height}
    , values_(buffer, buffer + (static_cast<int64_t>(width) * height))
{
}

Int16Frame::Int16Frame(const vector<int16_t> pixels, int width, int height)
    : width_{width}
    , height_{height}
    , values_(pixels)
{
}

Int16Frame Int16Frame::getDownsampled(int downsampling_factor) const
{
    int downsampled_width{width_ / downsampling_factor};
    int downsampled_height{height_ / downsampling_factor};
    std::vector<int16_t> downsampled_values(static_cast<int64_t>(downsampled_width) *
                                            downsampled_height);
    for (int row{0}; row < downsampled_height; ++row) {
        int downsampled_index{row * downsampled_width};
        int index{row * downsampling_factor * width_};
        for (int col{0}; col < downsampled_width; ++col) {
            downsampled_values[downsampled_index] = values_[index];
            ++downsampled_index;
            index += downsampling_factor;
        }
    }
    return Int16Frame{downsampled_values, downsampled_width, downsampled_height};
}
} // namespace tg
