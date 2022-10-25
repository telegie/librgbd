#include "ffmpeg_video_decoder.hpp"

#pragma warning(push)
#pragma warning(disable : 4244 26812)
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma warning(pop)

namespace rgbd
{
// A helper function for Vp8Decoder::decode() that feeds frames of packet into decoder_frames.
void decode_video_packet(AVCodecContext* codec_context,
                         AVPacket* packet,
                         std::vector<unique_ptr<YuvFrame>>& yuv_frames)
{
    if (avcodec_send_packet(codec_context, packet) < 0)
        throw std::runtime_error("Error from avcodec_send_packet.");

    while (true) {
        AVFrameHandle av_frame;
        av_frame->format = AV_PIX_FMT_YUV420P;

        int receive_frame_result = avcodec_receive_frame(codec_context, av_frame.get());
        if (receive_frame_result == AVERROR(EAGAIN) || receive_frame_result == AVERROR_EOF) {
            return;
        } else if (receive_frame_result < 0) {
            throw std::runtime_error("Error from avcodec_send_packet.");
        }

        yuv_frames.push_back(std::make_unique<YuvFrame>(av_frame));
    }
}

FFmpegVideoDecoder::FFmpegVideoDecoder(ColorCodecType type)
    : codec_parser_context_{find_decoder_avcodec(type)->id}
    , codec_context_{find_decoder_avcodec(type)}
    , packet_{}
{
    if (avcodec_open2(codec_context_.get(), nullptr, nullptr) < 0) {
        spdlog::error("avcodec_open2 failed");
        throw std::runtime_error("avcodec_open2 failed.");
    }
}

// Decode frames in vp8_frame_data.
unique_ptr<YuvFrame> FFmpegVideoDecoder::decode(gsl::span<const std::byte> vp8_frame)
{
    std::vector<unique_ptr<YuvFrame>> yuv_frames;
    /* use the parser to split the data into frames */
    size_t data_size{vp8_frame.size()};
    // Adding buffer padding is important!
    // Removing this will result in crashes in some cases.
    // When the crash happens, it happens in av_parser_parse2().
    std::unique_ptr<uint8_t> padded_data(new uint8_t[data_size + AV_INPUT_BUFFER_PADDING_SIZE]);
    memcpy(padded_data.get(), vp8_frame.data(), data_size);
    memset(padded_data.get() + data_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    uint8_t* data{padded_data.get()};

    while (data_size > 0) {
        // Returns the number of bytes used.
        int size{av_parser_parse2(codec_parser_context_.get(),
                                  codec_context_.get(),
                                  &packet_->data,
                                  &packet_->size,
                                  data,
                                  static_cast<int>(data_size),
                                  AV_NOPTS_VALUE,
                                  AV_NOPTS_VALUE,
                                  0)};

        if (size < 0)
            throw std::runtime_error("An error from av_parser_parse2.");

        data += size;
        data_size -= size;

        if (packet_->size)
            decode_video_packet(codec_context_.get(), packet_.get(), yuv_frames);
    }

    if (yuv_frames.size() != 1)
        throw std::runtime_error(
            "More or less than one frame found in FFmpegVideoDecoder::decode.");

    return std::move(yuv_frames[0]);
}
} // namespace tg
