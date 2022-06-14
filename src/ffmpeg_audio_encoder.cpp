#include "ffmpeg_audio_encoder.hpp"

namespace tg
{
FFmpegAudioEncoder::FFmpegAudioEncoder()
    : codec_context_{avcodec_find_encoder(AV_CODEC_ID_OPUS)}
    , frame_{}
    , next_pts_{0}
{
    codec_context_->sample_fmt = AV_SAMPLE_FMT_FLT;
    codec_context_->sample_rate = AUDIO_SAMPLE_RATE;
    codec_context_->bit_rate = 64000;
    codec_context_->channel_layout = AV_CH_LAYOUT_MONO;
    codec_context_->channels = av_get_channel_layout_nb_channels(codec_context_->channel_layout);

    if (avcodec_open2(codec_context_.get(), nullptr, nullptr) < 0) {
        spdlog::error("avcodec_open2 failed.");
        throw std::runtime_error("avcodec_open2 failed.");
    }

    if (codec_context_->frame_size != AUDIO_INPUT_SAMPLES_PER_FRAME)
        throw std::runtime_error("codec_context_->frame_size != AUDIO_INPUT_SAMPLES_PER_FRAME");

    frame_->format = codec_context_->sample_fmt;
    frame_->channel_layout = codec_context_->channel_layout;
    frame_->sample_rate = codec_context_->sample_rate;
    frame_->nb_samples = codec_context_->frame_size;

    if (av_frame_get_buffer(frame_.get(), 0) < 0)
        throw std::runtime_error("av_frame_get_buffer failed");
};

void FFmpegAudioEncoder::encode(gsl::span<const float> pcm_samples, vector<AVPacketHandle>& packets)
{
    // frame_->nb_samples gets set to 960 for opus in 48 kHz.
    if (pcm_samples.size() != AUDIO_INPUT_SAMPLES_PER_FRAME)
        throw std::runtime_error("pcm_samples.size() != AUDIO_INPUT_SAMPLES_PER_FRAME");

    if (av_frame_make_writable(frame_.get()) < 0)
        throw std::runtime_error("error from av_frame_make_writable");

    float* frame_data_ptr{reinterpret_cast<float*>(frame_->data[0])};
    for (int j{0}; j < frame_->nb_samples; ++j) {
        for (int i{0}; i < codec_context_->channels; ++i)
            *frame_data_ptr++ = pcm_samples[j];
    }

    frame_->pts = next_pts_;
    next_pts_ += frame_->nb_samples;

    encodeAudioFrame(codec_context_.get(), frame_.get(), packets);
}

void FFmpegAudioEncoder::flush(vector<AVPacketHandle>& packets)
{
    encodeAudioFrame(codec_context_.get(), nullptr, packets);
}

void FFmpegAudioEncoder::encodeAudioFrame(AVCodecContext* codec_context,
                                          AVFrame* frame,
                                          vector<AVPacketHandle>& packets)
{
    if (avcodec_send_frame(codec_context, frame) < 0)
        throw std::runtime_error("avcodec_send_frame failed");

    while (true) {
        AVPacketHandle packet;
        int result{avcodec_receive_packet(codec_context, packet.get())};
        if (frame && result == AVERROR(EAGAIN)) {
            return;
        } else if (!frame && result == AVERROR_EOF) {
            return;
        } else if (result < 0) {
            throw std::runtime_error("Error during encoding");
        }
        packets.push_back(packet);
    }
}
} // namespace tg
