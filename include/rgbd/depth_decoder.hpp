#pragma once

#include "integer_frame.hpp"

namespace rgbd
{
class DepthDecoderImpl
{
public:
    virtual ~DepthDecoderImpl() {}
    virtual unique_ptr<Int32Frame> decode(span<const byte> bytes) noexcept = 0;
};

class DepthDecoder
{
public:
    DepthDecoder(DepthCodecType depth_codec_type);
    unique_ptr<Int32Frame> decode(span<const byte> bytes) noexcept;

private:
    unique_ptr<DepthDecoderImpl> impl_;
};
} // namespace rgbd
