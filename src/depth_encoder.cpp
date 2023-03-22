#include "depth_encoder.hpp"

#include "rvl_encoder.hpp"
#include "tdc1_encoder.hpp"

namespace rgbd
{
DepthEncoder::DepthEncoder(DepthCodecType type, int width, int height)
    : impl_{}
{
    if (type == DepthCodecType::RVL) {
        impl_.reset(new RVLEncoder{width, height});
    } else if (type == DepthCodecType::TDC1) {
        // 500 as the default value.
        const int DEPTH_DIFF_MULTIPLIER{500};
        impl_.reset(new TDC1Encoder{width, height, DEPTH_DIFF_MULTIPLIER});
    } else {
        spdlog::error("Invalid type found in DepthEncoder::DepthEncoder: {}", type);
        throw std::runtime_error("Invalid type found in DepthEncoder::DepthEncoder");
    }
}

DepthCodecType DepthEncoder::getCodecType() noexcept
{
    return impl_->getCodecType();
}

Bytes DepthEncoder::encode(const int32_t* depth_values, bool keyframe) noexcept
{
    return impl_->encode(depth_values, keyframe);
}
} // namespace rgbd