#pragma once

#include "depth_decoder.hpp"
#include "rvl.hpp"

namespace rgbd
{
class RVLDecoder : public DepthDecoderImpl
{
public:
    RVLDecoder() noexcept;
    Int32Frame decode(gsl::span<const std::byte> bytes) noexcept;
};
}