#include "depth_decoder.hpp"

#include "rvl_decoder.hpp"
#include "tdc1_decoder.hpp"

namespace rgbd
{
DepthDecoder::DepthDecoder(DepthCodecType depth_codec_type)
    : impl_{}
{
    if (depth_codec_type == DepthCodecType::RVL) {
        impl_.reset(new RVLDecoder);
    } else if (depth_codec_type == DepthCodecType::TDC1) {
        impl_.reset(new TDC1Decoder);
    } else {
        spdlog::error("Invalid depth_codec_type found in DepthDecoder::DepthDecoder: {}", depth_codec_type);
        throw std::runtime_error("Invalid depth_codec_type found in DepthDecoder::DepthDecoder");
    }

}

unique_ptr<Int32Frame> DepthDecoder::decode(gsl::span<const std::byte> bytes) noexcept
{
    return impl_->decode(bytes);
}
}
