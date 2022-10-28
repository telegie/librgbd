#include "color_encoder.hpp"

#pragma warning(push)
#pragma warning(disable : 4244 26812)
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma warning(pop)

namespace rgbd
{
FFmpegVideoEncoder::FFmpegVideoEncoder(
    ColorCodecType type, int width, int height, int target_bitrate, int framerate)
    : codec_context_{find_encoder_avcodec(type)}
    , frame_{}
    , next_pts_{0}
{
    if (type != ColorCodecType::VP8) {
        spdlog::error("Invalid ColorCodecType");
        throw std::runtime_error("Invalid ColorCodecType");
    }

    AVDictionary* opt{nullptr};
    codec_context_->width = width;
    codec_context_->height = height;
    codec_context_->pix_fmt = AV_PIX_FMT_YUV420P;
    // setting codec_context_->rc_min_rate == codec_context_->rc_max_rate and
    // codec_context_->rc_min_rate == codec_context_->bit_rate
    // puts libvpx into VPX_CBR (constant bitrate) mode.
    codec_context_->bit_rate = target_bitrate * 1000;
    codec_context_->rc_min_rate = static_cast<int64_t>(target_bitrate) * 1000;
    codec_context_->rc_max_rate = target_bitrate * 1000;
    codec_context_->framerate = AVRational{framerate, 1};
    codec_context_->time_base = AVRational{1, framerate};

    codec_context_->thread_count = 4;
    codec_context_->profile = 1;
    av_dict_set_int(&opt, "lag-in-frames", 0, 0);
    codec_context_->qmin = 4;
    codec_context_->qmax = 56;
    // Set gop_size and keyint_min to INT_MAX as a way to
    // not have keyframes unless keyframe is set true in encode().
    codec_context_->gop_size = INT_MAX;
    codec_context_->keyint_min = INT_MAX;
     
    av_dict_set(&opt, "deadline", "realtime", 0);
    av_dict_set_int(&opt, "cpu-used", 4, 0);
    av_dict_set_int(&opt, "static-thresh", 0, 0);
    av_dict_set_int(&opt, "max-intra-rate", 300, 0);

    if (avcodec_open2(codec_context_.get(), nullptr, &opt) < 0) {
        spdlog::error("avcodec_open2 failed.");
        throw std::runtime_error("avcodec_open2 failed.");
    }

    frame_->width = codec_context_->width;
    frame_->height = codec_context_->height;
    frame_->format = codec_context_->pix_fmt;

    if (av_frame_get_buffer(frame_.get(), 0) < 0)
        throw std::runtime_error("av_frame_get_buffer failed");
};

unique_ptr<FFmpegVideoEncoderFrame> FFmpegVideoEncoder::encode(const YuvFrame& yuv_image, bool keyframe)
{
    return encode(yuv_image.y_channel().data(),
                  yuv_image.u_channel().data(),
                  yuv_image.v_channel().data(),
                  keyframe);
}

unique_ptr<FFmpegVideoEncoderFrame> FFmpegVideoEncoder::encode(const uint8_t* y_channel,
                                                               const uint8_t* u_channel,
                                                               const uint8_t* v_channel,
                                                               const bool keyframe)
{
    for (int row{0}; row < codec_context_->height; ++row) {
        int frame_row_index{row * frame_->linesize[0]};
        int y_row_index{row * codec_context_->width};
        for (int col{0}; col < codec_context_->width; ++col) {
            frame_->data[0][frame_row_index + col] = y_channel[y_row_index + col];
        }
    }

    int uv_width{codec_context_->width / 2};
    int uv_height{codec_context_->height / 2};
    for (int row{0}; row < uv_height; ++row) {
        int frame_u_row_index{row * frame_->linesize[1]};
        int frame_v_row_index{row * frame_->linesize[2]};
        int uv_row_index{row * codec_context_->width / 2};
        for (int col{0}; col < uv_width; ++col) {
            frame_->data[1][frame_u_row_index + col] = u_channel[uv_row_index + col];
            frame_->data[2][frame_v_row_index + col] = v_channel[uv_row_index + col];
        }
    }

    frame_->pts = next_pts_;
    if (keyframe) {
        frame_->key_frame = 1;
        frame_->pict_type = AV_PICTURE_TYPE_I;
    } else {
        frame_->key_frame = 0;
        frame_->pict_type = AV_PICTURE_TYPE_NONE;
    }

    auto packets{encodeVideoFrame(codec_context_.get(), frame_.get())};
    if (packets.size() != 1)
        throw std::runtime_error("Should be only one packet from one frame.");

    ++next_pts_;
    auto frame{std::make_unique<FFmpegVideoEncoderFrame>()};
    frame->packet = packets[0];
    return frame;
}

vector<AVPacketHandle> FFmpegVideoEncoder::encodeVideoFrame(AVCodecContext* codec_context, AVFrame* frame)
{
    vector<AVPacketHandle> packets;
    if (avcodec_send_frame(codec_context, frame) < 0)
        throw std::runtime_error("avcodec_send_frame failed");

    while (true) {
        AVPacketHandle packet;
        int result{avcodec_receive_packet(codec_context, packet.get())};
        if (frame && result == AVERROR(EAGAIN)) {
            return packets;
        } else if (!frame && result == AVERROR_EOF) {
            return packets;
        } else if (result < 0) {
            throw std::runtime_error("Error during encoding");
        }
        packets.push_back(packet);
    }
}
} // namespace tg
