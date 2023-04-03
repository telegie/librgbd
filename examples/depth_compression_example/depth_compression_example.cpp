#include <rgbd/rgbd.hpp>
#include <iostream>
#include <zstd.h>

namespace rgbd
{
void run()
{
    auto video_folder{VideoFolder::createFromDefaultPath()};
    if (!video_folder)
        throw std::runtime_error("No video_folder...");

    auto video_file_path{video_folder->runSelectFileCLI()};
    if (!video_file_path)
        throw std::runtime_error("No video_file_path...");

    rgbd::RecordParser parser{video_file_path->generic_string()};
    auto file{parser.parse(true, true)};

    size_t color_size{0};
    size_t depth_size{0};
    for (auto& video_frame : file->video_frames()) {
        color_size += video_frame.color_bytes().size();
        depth_size += video_frame.depth_bytes().size();
    }

    spdlog::info("color_size: {}", color_size);
    spdlog::info("depth_size: {}", depth_size);

    rgbd::TDC1Decoder decoder;
    vector<unique_ptr<rgbd::Int32Frame>> decoded_frames;
    for (auto& video_frame : file->video_frames())
        decoded_frames.push_back(decoder.decode(video_frame.depth_bytes()));

    rgbd::TDC1Encoder encoder{decoded_frames[0]->width(), decoded_frames[0]->height(), 500};
    // TODO: Test if encoded ones are containing the same information by decompressing and comparing them.
    vector<Bytes> tdc1_frames;
    vector<Bytes> tdc1_zstd_frames;

    size_t rvl_byte_size_sum{0};
    size_t tdc1_byte_size_sum{0};
    size_t tdc1_zstd_byte_size_sum{0};
    size_t uncompressed_byte_size_sum{0};
    bool first{true};
    for (auto& decoded_frame : decoded_frames) {
        Bytes rvl_frame{rvl::compress<int32_t>(decoded_frame->values())};

        Bytes tdc1_frame{encoder.encode(decoded_frame->values().data(), first)};
        tdc1_frames.push_back(tdc1_frame);

        Bytes zstd_frame(ZSTD_compressBound(tdc1_frame.size()));
        size_t zstd_frame_size{ZSTD_compress(
            zstd_frame.data(), zstd_frame.size(), tdc1_frame.data(), tdc1_frame.size(), 1)};
        zstd_frame.resize(zstd_frame_size);
        tdc1_zstd_frames.push_back(zstd_frame);

        rvl_byte_size_sum += rvl_frame.size();
        tdc1_byte_size_sum += tdc1_frame.size();
        tdc1_zstd_byte_size_sum += zstd_frame.size();
        uncompressed_byte_size_sum += decoded_frame->width() * decoded_frame->height() * sizeof(int32_t);
        first = false;
    }

    spdlog::info("rvl_byte_size_sum: {}", rvl_byte_size_sum);
    spdlog::info("tdc1_byte_size_sum: {}", tdc1_byte_size_sum);
    spdlog::info("tdc1_zstd_byte_size_sum: {}", tdc1_zstd_byte_size_sum);
    spdlog::info("uncompressed_byte_size_sum: {}", uncompressed_byte_size_sum);

    spdlog::info("RVL compression ratio: {}", static_cast<float>(uncompressed_byte_size_sum) / rvl_byte_size_sum);
    spdlog::info("TDC1 compression ratio: {}", static_cast<float>(uncompressed_byte_size_sum) / tdc1_byte_size_sum);
    spdlog::info("TDC1 + ZSTD compression ratio: {}", static_cast<float>(uncompressed_byte_size_sum) / tdc1_zstd_byte_size_sum);
}
}

int main()
{
    rgbd::run();
    return 0;
}
