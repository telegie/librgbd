#pragma once

#include "constants.hpp"
#include "integer_frame.hpp"

namespace rgbd
{
class DepthDecoder
{
public:
    virtual ~DepthDecoder() {}
    virtual Int16Frame decode(gsl::span<const std::byte> bytes) noexcept = 0;
};
} // namespace tg
