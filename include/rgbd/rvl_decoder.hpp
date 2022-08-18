#pragma once

#include "depth_decoder.hpp"
#include "rvl.hpp"

namespace rgbd
{
class RVLDecoder : public DepthDecoderImpl
{
public:
    RVLDecoder() noexcept;
    Int16Frame decode(gsl::span<const std::byte> bytes) noexcept;

private:
    vector<int16_t> previous_depth_values_;
};
}