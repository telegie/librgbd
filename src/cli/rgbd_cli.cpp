#include <rgbd/file_parser.hpp>
#include <cxxopts.hpp>
#include <fstream>
#include <cli/cli.h>
#include <cli/loopscheduler.h>
#include <cli/clilocalsession.h>
#include <rgbd/video_folder.hpp>

void print_help(const cxxopts::Options& options)
{
    std::cout << options.help() << std::endl;
}

void print_file_info(std::ostream& out, const std::string& file_path)
{
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
    out << fmt::format("Total video frame count: {}\n", file->video_frames().size());
    out << fmt::format("Color track codec: {}\n", file_info.color_track_info().codec);
    out << fmt::format("Color track size: {} KB\n", color_byte_size / 1024);
    out << fmt::format("Depth track codec: {}\n", file_info.depth_track_info().codec);
    out << fmt::format("Depth track size: {} KB\n", depth_byte_size / 1024);

    if (file_info.depth_confidence_track_info()) {
        out << fmt::format("Depth confidence track codec: {}\n", file_info.depth_confidence_track_info()->codec);
        out << fmt::format("Depth confidence size: {} KB\n", depth_confidence_byte_size / 1024);
    } else {
        out << "No depth confidence track." << std::endl;
    }
}

void extract_cover(const std::string& file_path)
{
    rgbd::FileParser parser{file_path};
    auto& file_info{parser.info()};
    auto file{parser.readAll()};
    auto& cover_png_bytes{file_info.cover_png_bytes()};
    std::ofstream fout;
    fout.open("librgbd_cover.png", std::ios::binary | std::ios::out);
    fout.write((const char*)cover_png_bytes.data(), cover_png_bytes.size());
    fout.close();
}

void split_file()
{

}

int main(int argc, char** argv)
{
    cxxopts::Options options{"rgbd-cli", "CLI for librgbd."};
    options.add_option("", cxxopts::Option{"h,help", "Print Usage"});
    options.add_option("", cxxopts::Option{"i,info",
                                           "Print File Info",
                                           cxxopts::value<std::string>()});
    options.add_option("", cxxopts::Option{"c,cover",
                                           "Extract cover.png",
                                           cxxopts::value<std::string>()});

    auto result{options.parse(argc, argv)};
    if (result.count("help")) {
        print_help(options);
        return 0;
    } else if (result.count("info")) {
        auto file_path{result["info"].as<std::string>()};
        print_file_info(std::cout, file_path);
        return 0;
    } else if (result.count("cover")) {
        auto file_path{result["cover"].as<std::string>()};
        extract_cover(file_path);
        return 0;
    }

    std::cout << "Starting interactive mode." << std::endl;
    // create a menu (this is the root menu of our cli)
    auto root_menu = std::make_unique<cli::Menu>("rgbd-cli");

    root_menu->Insert("info", [](std::ostream& out){
        auto video_folder{rgbd::VideoFolder::createFromDefaultPath()};
        auto file_path{video_folder->runSelectFileCLI()};
        print_file_info(out, file_path->generic_u8string());
    });
    root_menu->Insert("cover", [](std::ostream& out){
        auto video_folder{rgbd::VideoFolder::createFromDefaultPath()};
        auto file_path{video_folder->runSelectFileCLI()};
        extract_cover(file_path->generic_u8string());
    });

    // create the cli with the root menu
    cli::Cli cli(std::move(root_menu));

    cli::LoopScheduler scheduler;
    cli::CliLocalTerminalSession localSession(cli, scheduler, std::cout);
    // start the scheduler main loop
    // it will exit from this method only when scheduler.Stop() is called
    // each cli callback will be executed in this thread
    scheduler.Run();

    return 0;
}
