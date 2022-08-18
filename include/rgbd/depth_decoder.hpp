#pragma once

#include "constants.hpp"
#include "integer_frame.hpp"

namespace rgbd
{
class DepthDecoderImpl
{
public:
    virtual ~DepthDecoderImpl() {}
    virtual Int32Frame decode(gsl::span<const std::byte> bytes) noexcept = 0;
};

class DepthDecoder
{
public:
    DepthDecoder(DepthCodecType depth_codec_type);
    Int32Frame decode(gsl::span<const std::byte> bytes) noexcept;

private:
    unique_ptr<DepthDecoderImpl> impl_;
};
} // namespace tg
