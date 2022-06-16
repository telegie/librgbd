/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "depth_decoder.hpp"
#include "rvl.hpp"

namespace tg
{
class TDC1Decoder : public DepthDecoder
{
public:
    TDC1Decoder() noexcept;
    Int16Frame decode(gsl::span<const std::byte> bytes) noexcept;

private:
    // Using int16_t to be compatible with the differences that can have
    // negative values.
    vector<int16_t> previous_depth_values_;
};
} // namespace tg
