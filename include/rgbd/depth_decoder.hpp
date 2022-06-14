#pragma once

#include "constants.hpp"
#include "int16_frame.hpp"

namespace tg
{
class DepthDecoder
{
public:
    virtual ~DepthDecoder() {}
    virtual Int16Frame decode(gsl::span<const std::byte> bytes) noexcept = 0;
};
} // namespace tg
