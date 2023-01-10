#pragma once

#include "depth_decoder.hpp"

namespace rgbd
{
class RVLDecoder : public DepthDecoderImpl
{
public:
    RVLDecoder() noexcept;
    unique_ptr<Int32Frame> decode(gsl::span<const std::byte> bytes) noexcept;
};
}