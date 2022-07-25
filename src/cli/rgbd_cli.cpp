#include <rgbd/file_parser.hpp>
#include <cxxopts.hpp>
#include <fstream>

int main(int argc, char** argv)
{
    cxxopts::Options options{"rgbd-cli", "CLI for librgbd."};
    options.add_option("", cxxopts::Option{"h,help", "Print Usage"});
    options.add_option("", cxxopts::Option{"f,file",
                                           "Print File Details",
                                           cxxopts::value<std::string>()});
    options.add_option("", cxxopts::Option{"c,cover",
                                           "Cover Art File",
                                           cxxopts::value<std::string>()});

    auto result{options.parse(argc, argv)};
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    } else if (result.count("file")) {
        auto file_path{result["file"].as<std::string>()};
        rgbd::FileParser parser{file_path};
        auto& file_info{parser.info()};
        auto file{parser.readAll()};
        size_t color_byte_size{0};
        size_t depth_byte_size{0};
        size_t depth_confidence_byte_size{0};
        for (auto& video_frame : file->video_frames()) {
            color_byte_size += video_frame->color_bytes().size();
            depth_byte_size += video_frame->depth_bytes().size();
            auto& depth_confidence_bytes{video_frame->depth_confidence_bytes()};
            if (depth_confidence_bytes)
                depth_confidence_byte_size += depth_confidence_bytes->size();
        }
        std::cout << fmt::format("Total video frame count: {}\n", file->video_frames().size());
        std::cout << fmt::format("Color track codec: {}\n", file_info.color_track_info().codec);
        std::cout << fmt::format("Color track size: {} KB\n", color_byte_size / 1024);
        std::cout << fmt::format("Depth track codec: {}\n", file_info.depth_track_info().codec);
        std::cout << fmt::format("Depth track size: {} KB\n", depth_byte_size / 1024);

        if (file_info.depth_confidence_track_info()) {
            std::cout << fmt::format("Depth confidence track codec: {}\n", file_info.depth_confidence_track_info()->codec);
            std::cout << fmt::format("Depth confidence size: {} KB\n", depth_confidence_byte_size / 1024);
        } else {
            std::cout << "No depth confidence track." << std::endl;
        }
        return 0;
    } else if (result.count("cover")) {
        auto file_path{result["cover"].as<std::string>()};
        rgbd::FileParser parser{file_path};
        auto& file_info{parser.info()};
        auto file{parser.readAll()};
        auto& cover_png_bytes{file_info.cover_png_bytes()};
        std::ofstream fout;
        fout.open("librgbd_cover.png", std::ios::binary | std::ios::out);
        fout.write((const char*)cover_png_bytes.data(), cover_png_bytes.size());
        fout.close();
    }

    std::cout << "no option found from rgbd-cli" << std::endl;
    return 0;
}
