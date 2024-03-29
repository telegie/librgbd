#include "tdc1_encoder.hpp"

#include "byte_utils.hpp"

namespace rgbd
{
TDC1Encoder::TDC1Encoder(int width, int height, int diff_multiplier) noexcept
    : width_{width}
    , height_{height}
    , diff_multiplier_{diff_multiplier}
    , previous_depth_values_(static_cast<size_t>(width) * height, 0)
{
}

DepthCodecType TDC1Encoder::getCodecType() noexcept
{
    return DepthCodecType::TDC1;
}

Bytes TDC1Encoder::encode(const int32_t* depth_values, const bool keyframe) noexcept
{
    Bytes bytes;
    append_bytes(bytes, convert_to_bytes(width_));
    append_bytes(bytes, convert_to_bytes(height_));
    append_bytes(bytes, convert_to_bytes(static_cast<int32_t>(keyframe)));

    auto depth_value_count{previous_depth_values_.size()};
    if (keyframe) {
        for (gsl::index i{0}; i < depth_value_count; ++i) {
            previous_depth_values_[i] = depth_values[i];
        }

        append_bytes(
            bytes, rvl::compress(span<const int32_t>{depth_values, previous_depth_values_.size()}));
        return bytes;
    }

    vector<int32_t> depth_value_diffs(depth_value_count);
    for (size_t i{0}; i < depth_value_count; ++i) {
        int diff{depth_values[i] - previous_depth_values_[i]};
        if ((std::abs(diff) * diff_multiplier_) > previous_depth_values_[i]) {
            previous_depth_values_[i] = depth_values[i];
            depth_value_diffs[i] = diff;
        } else {
            depth_value_diffs[i] = 0;
        }
    }

    append_bytes(bytes, rvl::compress<int32_t>(depth_value_diffs));
    return bytes;
}
} // namespace rgbd
