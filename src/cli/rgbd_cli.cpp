#include <cli/cli.h>
#include <cli/clilocalsession.h>
#include <cli/loopscheduler.h>
#include <cxxopts.hpp>
#include <fstream>
#include <rgbd/file_parser.hpp>
#include <rgbd/video_folder.hpp>
#include <rgbd/file_writer.hpp>
#include <rgbd/ffmpeg_video_decoder.hpp>
#include <rgbd/tdc1_decoder.hpp>

namespace rgbd
{
void print_help(const cxxopts::Options& options)
{
    std::cout << options.help() << std::endl;
}

void print_file_info(std::ostream& out, const std::string& file_path)
{
    FileParser parser{file_path};
    auto file{parser.parseNoFrames()};
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
    out << fmt::format("Color track codec: {}\n", file->tracks().color_track.codec);
    out << fmt::format("Color track size: {} KB\n", color_byte_size / 1024);
    out << fmt::format("Depth track codec: {}\n", file->tracks().depth_track.codec);
    out << fmt::format("Depth track size: {} KB\n", depth_byte_size / 1024);

    if (file->tracks().depth_confidence_track) {
        out << fmt::format("Depth confidence track codec: {}\n",
                           file->tracks().depth_confidence_track->codec);
        out << fmt::format("Depth confidence size: {} KB\n", depth_confidence_byte_size / 1024);
    } else {
        out << "No depth confidence track." << std::endl;
    }
}

void extract_cover(const std::string& file_path)
{
    FileParser parser{file_path};
    auto file{parser.parseNoFrames()};
    auto& cover_png_bytes{file->attachments().cover_png_bytes};
    if (!cover_png_bytes) {
        spdlog::error("No cover.png found.");
        return;
    }
    std::ofstream fout;
    fout.open("librgbd_cover.png", std::ios::binary | std::ios::out);
    fout.write((const char*)cover_png_bytes->data(), cover_png_bytes->size());
    fout.close();
}

void split_file(const std::string& file_path)
{
    FileParser parser{file_path};
    auto file{parser.parseAllFrames()};
    auto& video_frames{file->video_frames()};

    int previous_chunk_index{-1};
    unique_ptr<FileWriter> file_writer;
    unique_ptr<FFmpegVideoEncoder> color_encoder;
    unique_ptr<DepthEncoder> depth_encoder;

    FFmpegVideoDecoder color_decoder{ColorCodecType::VP8};
    TDC1Decoder depth_decoder;

    for (auto& video_frame : video_frames) {
        int64_t global_timecode{video_frame->global_timecode()};
        constexpr int TWO_SECONDS{2000000};
        int chunk_index{gsl::narrow<int>(global_timecode / TWO_SECONDS)};

        YuvFrame color_frame{color_decoder.decode(video_frame->color_bytes())};
        Int32Frame depth_frame{depth_decoder.decode(video_frame->depth_bytes())};

        bool first{false};
        if (chunk_index == previous_chunk_index + 1) {
            if (file_writer)
                file_writer->flush();

            auto output_path{fmt::format("chunk_{}.mkv", chunk_index)};
            file_writer = std::make_unique<FileWriter>(
                output_path,
                false,
                *file->attachments().camera_calibration,
                30,
                DepthCodecType::TDC1,
                static_cast<int>(file->tracks().audio_track.sampling_frequency));

            color_encoder = std::make_unique<FFmpegVideoEncoder>(ColorCodecType::VP8,
                                                                 color_frame.width(),
                                                                 color_frame.height(),
                                                                 2500,
                                                                 30);
            depth_encoder = DepthEncoder::createTDC1Encoder(depth_frame.width(),
                                                            depth_frame.height(),
                                                            500);
            first = true;

            file_writer->writeCover(color_frame);
            previous_chunk_index = chunk_index;
        } else if (chunk_index == previous_chunk_index) {
            first = false;
        } else {
            throw std::runtime_error("Invalid chunk_index found...");
        }

        auto encoded_color_frame{color_encoder->encode(color_frame, first)};
        auto encoded_depth_frame{depth_encoder->encode(depth_frame.values(), first)};

        file_writer->writeVideoFrame(video_frame->global_timecode(),
                                     encoded_color_frame->packet.getDataBytes(),
                                     encoded_depth_frame,
                                     nullopt,
                                     video_frame->floor());
    }

    file_writer->flush();
}

int main(int argc, char** argv)
{
    cxxopts::Options options{"rgbd-cli", "CLI for librgbd."};
    options.add_option("", cxxopts::Option{"h,help", "Print Usage"});
    options.add_option("",
                       cxxopts::Option{"i,info", "Print File Info", cxxopts::value<std::string>()});
    options.add_option(
        "", cxxopts::Option{"c,cover", "Extract cover.png", cxxopts::value<std::string>()});

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

    root_menu->Insert("info", [](std::ostream& out) {
        auto video_folder{rgbd::VideoFolder::createFromDefaultPath()};
        auto file_path{video_folder->runSelectFileCLI()};
        print_file_info(out, file_path->generic_u8string());
    });
    root_menu->Insert("cover", [](std::ostream& out) {
        auto video_folder{rgbd::VideoFolder::createFromDefaultPath()};
        auto file_path{video_folder->runSelectFileCLI()};
        extract_cover(file_path->generic_u8string());
    });
    root_menu->Insert("split", [](std::ostream& out) {
        auto video_folder{rgbd::VideoFolder::createFromDefaultPath()};
        auto file_path{video_folder->runSelectFileCLI()};
        split_file(file_path->generic_u8string());
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
} // namespace rgbd

int main(int argc, char** argv)
{
    return rgbd::main(argc, argv);
}
