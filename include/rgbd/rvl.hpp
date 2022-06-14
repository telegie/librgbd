/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "constants.hpp"

// This algorithm is from
// Wilson, A. D. (2017, October). Fast lossless depth image compression.
// In Proceedings of the 2017 ACM International Conference on Interactive
// Surfaces and Spaces (pp. 100-105). ACM.
namespace tg
{
namespace rvl
{
// It has to be int16_t not uint16_t to work with TRVL.
Bytes compress(gsl::span<const int16_t> input) noexcept;
vector<int16_t> decompress(gsl::span<const std::byte> input, int num_pixels) noexcept;
} // namespace rvl
} // namespace tg
