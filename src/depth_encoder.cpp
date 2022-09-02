#include "depth_encoder.hpp"

#include "rvl_encoder.hpp"
#include "tdc1_encoder.hpp"

namespace rgbd
{
DepthEncoder::DepthEncoder(unique_ptr<DepthEncoderImpl>&& impl)
    : impl_{std::move(impl)}
{
}

unique_ptr<DepthEncoder> DepthEncoder::createRVLEncoder(int width, int height)
{
    auto instance{new DepthEncoder{std::make_unique<RVLEncoder>(width, height)}};
    return unique_ptr<DepthEncoder>(instance);
}

unique_ptr<DepthEncoder>
DepthEncoder::createTDC1Encoder(int width, int height, int depth_diff_multiplier)
{
    auto instance{
        new DepthEncoder{std::make_unique<TDC1Encoder>(width, height, depth_diff_multiplier)}};
    return unique_ptr<DepthEncoder>(instance);
}

DepthCodecType DepthEncoder::getCodecType() noexcept
{
    return impl_->getCodecType();
}

Bytes DepthEncoder::encode(gsl::span<const int32_t> depth_values, bool keyframe) noexcept
{
    return impl_->encode(depth_values, keyframe);
}
} // namespace rgbd