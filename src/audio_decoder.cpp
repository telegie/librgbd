#include "audio_decoder.hpp"

#include <iostream>
extern "C"
{
#include <libavformat/avformat.h>
}

namespace rgbd
{
vector<float> get_pcm_samples(AVFrameHandle& av_frame)
{
    vector<float> pcm_samples;
    float* data_ptr{reinterpret_cast<float*>(av_frame->extended_data[0])};
    for (int i{0}; i < av_frame->nb_samples; ++i)
        pcm_samples.push_back(data_ptr[i]);

    return pcm_samples;
}

// A helper function for Vp8Decoder::decode() that feeds frames of packet into decoder_frames.
void decode_audio_packet(AVCodecContext* codec_context,
                         AVPacket* packet,
                         std::vector<float>& pcm_samples)
{
    if (avcodec_send_packet(codec_context, packet) < 0)
        throw std::runtime_error("Error from avcodec_send_packet.");

    while (true) {
        AVFrameHandle av_frame;
        av_frame->format = AV_SAMPLE_FMT_FLT;

        int receive_frame_result = avcodec_receive_frame(codec_context, av_frame.get());
        if (receive_frame_result == AVERROR(EAGAIN) || receive_frame_result == AVERROR_EOF) {
            return;
        } else if (receive_frame_result < 0) {
            throw std::runtime_error("Error from avcodec_send_packet.");
        }
        
        for (float pcm_sample : get_pcm_samples(av_frame))
            pcm_samples.push_back(pcm_sample);
    }
}

AudioDecoder::AudioDecoder()
    : codec_parser_context_{avcodec_find_decoder(AV_CODEC_ID_OPUS)->id}
    , codec_context_{avcodec_find_decoder(AV_CODEC_ID_OPUS)}
    , packet_{}
{
    codec_context_->request_sample_fmt = AV_SAMPLE_FMT_FLT;
    codec_context_->channels = 1;

    if (avcodec_open2(codec_context_.get(), nullptr, nullptr) < 0) {
        spdlog::error("avcodec_open2 failed");
        throw std::runtime_error("avcodec_open2 failed.");
    }
}

// Decode frames in vp8_frame_data.
vector<float> AudioDecoder::decode(span<const byte> opus_frame)
{
    vector<float> pcm_samples;
    /* use the parser to split the data into frames */
    size_t data_size{opus_frame.size()};
    // Adding buffer padding is important!
    // Removing this will result in crashes in some cases.
    // When the crash happens, it happens in av_parser_parse2().
    unique_ptr<uint8_t> padded_data(new uint8_t[data_size + AV_INPUT_BUFFER_PADDING_SIZE]);
    memcpy(padded_data.get(), opus_frame.data(), data_size);
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
            decode_audio_packet(codec_context_.get(), packet_.get(), pcm_samples);
    }

    return pcm_samples;
}
} // namespace rgbd
