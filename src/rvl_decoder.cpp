#include "rvl_decoder.hpp"

#include "byte_utils.hpp"
#include "rvl.hpp"

namespace rgbd
{
RVLDecoder::RVLDecoder() noexcept {}

unique_ptr<Int32Frame> RVLDecoder::decode(gsl::span<const std::byte> bytes) noexcept
{
    int cursor{0};
    int width{read_from_bytes<int32_t>(bytes, cursor)};
    int height{read_from_bytes<int32_t>(bytes, cursor)};
    gsl::span<const std::byte> encoded_depth_values{bytes.data() + cursor, bytes.size() - cursor};

    return std::make_unique<Int32Frame>(width, height,
        rvl::decompress<int32_t>(encoded_depth_values, width * height));
}
} // namespace rgbd