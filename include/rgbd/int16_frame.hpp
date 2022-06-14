#pragma once

#include "constants.hpp"

namespace tg
{
class Int16Frame
{
public:
    Int16Frame(const int16_t* buffer, int width, int height);
    Int16Frame(const vector<int16_t> pixels, int width, int height);
    Int16Frame getDownsampled(int downsampling_factor) const;
    int width() const noexcept
    {
        return width_;
    }
    int height() const noexcept
    {
        return height_;
    }
    std::vector<int16_t>& values() noexcept
    {
        return values_;
    }
    const std::vector<int16_t>& values() const noexcept
    {
        return values_;
    }

private:
    int width_;
    int height_;
    std::vector<int16_t> values_;
};
}
