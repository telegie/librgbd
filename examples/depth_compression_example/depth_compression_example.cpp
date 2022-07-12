#include <rgbd/rgbd.hpp>
#include <iostream>
#include <zstd.h>

namespace rgbd
{
void run()
{
    const std::vector<std::string> VIDEO_FOLDER_PATHS{"../../../../../../videos/",
                                                      "../../../../../../../videos/"};
    auto videos_folder{FileSystemUtils::findDataFolder(VIDEO_FOLDER_PATHS)};
    if (!videos_folder)
        throw std::runtime_error("No videos folder...");

    std::cout << "Options:" << std::endl;
    for (int i{0}; i < videos_folder->filenames.size(); ++i)
        std::cout << "- (" << i << ") " << videos_folder->filenames[i] << std::endl;

    int index{0};
    while (true) {
        std::cout << "Select: ";
        string line;
        std::getline(std::cin, line);
        try {
            index = stoi(line);
            if (index >= 0 && index < videos_folder->filenames.size())
                break;
        } catch (std::invalid_argument) {
            // Ignore non-numeric inputs.
        }
        std::cout << "Invalid Input...\n";
    }

    const auto filename{videos_folder->filenames[index]};
    const auto file_path{videos_folder->folder_path + filename};

    rgbd::FileParser parser{file_path.c_str()};
    auto file{parser.readAll()};

    size_t color_size{0};
    size_t depth_size{0};
    for (auto& video_frame : file->video_frames()) {
        color_size += video_frame->color_bytes().size();
        depth_size += video_frame->depth_bytes().size();
    }

    spdlog::info("color_size: {}", color_size);
    spdlog::info("depth_size: {}", depth_size);

    rgbd::TDC1Decoder decoder;
    vector<rgbd::Int16Frame> decoded_frames;
    for (auto& video_frame : file->video_frames())
        decoded_frames.push_back(decoder.decode(video_frame->depth_bytes()));

    rgbd::TDC1Encoder encoder{decoded_frames[0].width(), decoded_frames[0].height(), 500};
    vector<Bytes> encoded_frames1;
    vector<Bytes> encoded_frames2;
    bool first{true};
    for (auto& decoded_frame : decoded_frames) {
        Bytes encoded_frame{encoder.encode(decoded_frame.values(), first)};
        encoded_frames1.push_back(encoded_frame);
        Bytes zstd_frame{ZSTD_compressBound(encoded_frame.size())};
        size_t zstd_frame_size{ZSTD_compress(
            zstd_frame.data(), zstd_frame.size(), encoded_frame.data(), encoded_frame.size(), 1)};
        zstd_frame.resize(zstd_frame_size);
        encoded_frames2.push_back(zstd_frame);
        first = false;
    }
    spdlog::info("encoded_frames1.size(): {}", encoded_frames1.size());
    spdlog::info("encoded_frames2.size(): {}", encoded_frames2.size());

    size_t encoder1_size{0};
    size_t encoder2_size{0};
    for (auto& encoded_frame1 : encoded_frames1)
        encoder1_size += encoded_frame1.size();
    for (auto& encoded_frame2 : encoded_frames2)
        encoder2_size += encoded_frame2.size();

    spdlog::info("encoder1_size: {}", encoder1_size);
    spdlog::info("encoder2_size: {}", encoder2_size);
}
}

int main()
{
    rgbd::run();
    return 0;
}
