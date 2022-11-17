/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "tdc1_decoder.hpp"

#include "byte_utils.hpp"

namespace rgbd
{
bool is_tdc1_keyframe(gsl::span<const std::byte> bytes)
{
    int cursor{8};
    bool keyframe{read_from_bytes<int32_t>(bytes, cursor) > 0 ? true : false};
    return keyframe;
}

TDC1Decoder::TDC1Decoder() noexcept
    : previous_depth_values_{}
{
}

unique_ptr<Int32Frame> TDC1Decoder::decode(gsl::span<const std::byte> bytes) noexcept
{
    int cursor{0};
    int width{read_from_bytes<int32_t>(bytes, cursor)};
    int height{read_from_bytes<int32_t>(bytes, cursor)};
    bool keyframe{read_from_bytes<int32_t>(bytes, cursor) > 0 ? true : false};
    gsl::span<const std::byte> encoded_depth_values{bytes.data() + cursor, bytes.size() - cursor};

    if (previous_depth_values_.size() == 0)
        previous_depth_values_ = vector<int32_t>(static_cast<int64_t>(width) * height, 0);

    const int depth_value_count{gsl::narrow<int>(previous_depth_values_.size())};
    if (keyframe) {
        previous_depth_values_ = rvl::decompress<int32_t>(encoded_depth_values, depth_value_count);
        return std::make_unique<Int32Frame>(width, height, previous_depth_values_);
    }

    const auto depth_value_diffs{rvl::decompress<int32_t>(encoded_depth_values, depth_value_count)};
    for (gsl::index i{0}; i < depth_value_count; ++i)
        previous_depth_values_[i] += depth_value_diffs[i];

    return std::make_unique<Int32Frame>(width, height, previous_depth_values_);
}
} // namespace rgbd
