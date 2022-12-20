#include <cli/cli.h>
#include <cli/clilocalsession.h>
#include <cli/loopscheduler.h>
#include <cxxopts.hpp>
#include <fstream>
#include <rgbd/color_decoder.hpp>
#include <rgbd/file_parser.hpp>
#include <rgbd/file_writer.hpp>
#include <rgbd/tdc1_decoder.hpp>
#include <rgbd/undistorted_camera_calibration.hpp>
#include <rgbd/video_folder.hpp>

namespace rgbd
{
void print_help(const cxxopts::Options& options)
{
    std::cout << options.help() << std::endl;
}

void print_file_info(std::ostream& out, const std::string& file_path)
{
    FileParser parser{file_path};
    auto file{parser.parse(false, false)};
    size_t color_byte_size{0};
    size_t depth_byte_size{0};
    for (auto& video_frame : file->video_frames()) {
        color_byte_size += video_frame->color_bytes().size();
        depth_byte_size += video_frame->depth_bytes().size();
    }
    out << fmt::format("Total video frame count: {}\n", file->video_frames().size());
    out << fmt::format("Color codec: {}\n", file->tracks().color_track.codec);
    out << fmt::format("Color width: {}\n", file->tracks().color_track.width);
    out << fmt::format("Color height: {}\n", file->tracks().color_track.height);
    out << fmt::format("Color byte size: {} KB\n", color_byte_size / 1024);
    out << fmt::format("Depth codec: {}\n", file->tracks().depth_track.codec);
    out << fmt::format("Color width: {}\n", file->tracks().depth_track.width);
    out << fmt::format("Color height: {}\n", file->tracks().depth_track.height);
    out << fmt::format("Depth byte size: {} KB\n", depth_byte_size / 1024);

    auto device_type{file->attachments().camera_calibration->getCameraDeviceType()};
    out << fmt::format("Camera Device Type: {}\n", stringify_camera_device_type(device_type));
}

void extract_cover(const std::string& file_path)
{
    FileParser parser{file_path};
    auto file{parser.parse(false, false)};
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
    auto file{parser.parse(false, false)};
    auto& video_frames{file->video_frames()};

    int previous_chunk_index{-1};
    unique_ptr<FileWriter> file_writer;
    unique_ptr<ColorEncoder> color_encoder;
    unique_ptr<DepthEncoder> depth_encoder;

    ColorDecoder color_decoder{ColorCodecType::VP8};
    TDC1Decoder depth_decoder;

    for (auto& video_frame : video_frames) {
        int64_t global_timecode{video_frame->global_timecode()};
        constexpr int TWO_SECONDS{2000000};
        int chunk_index{gsl::narrow<int>(global_timecode / TWO_SECONDS)};

        auto color_frame{color_decoder.decode(video_frame->color_bytes())};
        auto depth_frame{depth_decoder.decode(video_frame->depth_bytes())};

        bool first{false};
        if (chunk_index == previous_chunk_index + 1) {
            if (file_writer)
                file_writer->flush();

            auto output_path{fmt::format("chunk_{}.mkv", chunk_index)};
            FileWriterConfig writer_config;
            writer_config.depth_codec_type = DepthCodecType::TDC1;
            file_writer = std::make_unique<FileWriter>(
                output_path, *file->attachments().camera_calibration, writer_config);

            color_encoder = std::make_unique<ColorEncoder>(
                ColorCodecType::VP8, color_frame->width(), color_frame->height(), 2500, 30);
            depth_encoder =
                DepthEncoder::createTDC1Encoder(depth_frame->width(), depth_frame->height(), 500);
            first = true;

            file_writer->writeCover(*color_frame);
            previous_chunk_index = chunk_index;
        } else if (chunk_index == previous_chunk_index) {
            first = false;
        } else {
            throw std::runtime_error("Invalid chunk_index found...");
        }

        auto color_bytes{color_encoder->encode(*color_frame, first)};
        auto depth_bytes{depth_encoder->encode(depth_frame->values(), first)};

        file_writer->writeVideoFrame(video_frame->global_timecode(),
                                     first,
                                     color_bytes,
                                     depth_bytes);
    }

    file_writer->flush();
}

void trim_file(const std::string& file_path, float from_sec, float to_sec)
{
    int64_t from_us{static_cast<int64_t>(from_sec * 1000000)};
    int64_t to_us{static_cast<int64_t>(to_sec * 1000000)};
    FileParser parser{file_path};
    auto file{parser.parse(true, true)};
    auto& video_frames{file->video_frames()};

    auto output_path{"trimmed.mkv"};
    FileWriterConfig writer_config;
    writer_config.depth_codec_type = DepthCodecType::TDC1;
    FileWriter file_writer{output_path, *file->attachments().camera_calibration, writer_config};

    ColorEncoder color_encoder{ColorCodecType::VP8,
                               file->tracks().color_track.width,
                               file->tracks().color_track.height,
                               2500,
                               30};
    unique_ptr<DepthEncoder> depth_encoder{DepthEncoder::createTDC1Encoder(
        file->tracks().depth_track.width, file->tracks().depth_track.height, 500)};

    ColorDecoder color_decoder{ColorCodecType::VP8};
    TDC1Decoder depth_decoder;

    int previous_keyframe_index{-1};
    for (auto& video_frame : video_frames) {
        int64_t original_global_timecode{video_frame->global_timecode()};
        if (original_global_timecode < from_us)
            continue;
        if (original_global_timecode > to_us)
            break;

        int64_t trimmed_global_timecode{video_frame->global_timecode() - from_us};
        constexpr int TWO_SECONDS{2000000};
        int keyframe_index{gsl::narrow<int>(trimmed_global_timecode / TWO_SECONDS)};

        auto color_frame{color_decoder.decode(video_frame->color_bytes())};
        auto depth_frame{depth_decoder.decode(video_frame->depth_bytes())};

        bool keyframe{false};
        if (keyframe_index == previous_keyframe_index + 1) {
            if (keyframe_index == 0)
                file_writer.writeCover(*color_frame);

            keyframe = true;
            previous_keyframe_index = keyframe_index;
        } else if (keyframe_index != previous_keyframe_index) {
            throw std::runtime_error("Invalid keyframe_index found...");
        }

        auto color_bytes{color_encoder.encode(*color_frame, keyframe)};
        auto depth_bytes{depth_encoder->encode(depth_frame->values(), keyframe)};

        file_writer.writeVideoFrame(trimmed_global_timecode,
                                    keyframe,
                                    color_bytes,
                                    depth_bytes);
    }

    file_writer.flush();
}

vector<optional<int>> get_index_map(const CameraCalibration& from_calibration,
                                    int from_width,
                                    int from_height,
                                    const CameraCalibration& to_calibration,
                                    int to_width,
                                    int to_height)
{
    // For mapping from from_calibration indices to to_calibration indices.
    vector<optional<int>> index_map(from_width * from_height);
    for (int from_row{0}; from_row < from_height; ++from_row) {
        for (int from_col{0}; from_col < from_width; ++from_col) {
            float from_u{from_col / static_cast<float>(from_width - 1)};
            float from_v{from_row / static_cast<float>(from_height - 1)};

            auto direction{from_calibration.getDirection(glm::vec2{from_u, from_v})};
            auto to_uv{to_calibration.getUv(direction)};
            int to_col{static_cast<int>(std::round(to_uv.x * (to_width - 1)))};
            int to_row{static_cast<int>(std::round(to_uv.y * (to_height - 1)))};

            int from_index{from_col + from_row * from_width};
            if (to_col < 0 || to_col >= to_width || to_row < 0 || to_row >= to_height) {
                index_map[from_index] = nullopt;
            } else {
                index_map[from_index] = to_col + to_row * to_width;
            }
        }
    }
    return index_map;
}

void standardize_calibration(const std::string& file_path)
{
    FileParser parser{file_path};
    auto file{parser.parse(true, true)};
    auto& video_frames{file->video_frames()};

    auto output_path{"standardized.mkv"};
    FileWriterConfig writer_config;
    writer_config.depth_codec_type = DepthCodecType::TDC1;

    auto& original_calibration{*file->attachments().camera_calibration};
    UndistortedCameraCalibration standard_calibration{1024, 1024, 512, 512, 0.5f, -0.5f, 0.5f, 0.5f};

    vector<optional<int>> y_index_map{get_index_map(standard_calibration,
                                                    standard_calibration.getColorWidth(),
                                                    standard_calibration.getColorHeight(),
                                                    original_calibration,
                                                    original_calibration.getColorWidth(),
                                                    original_calibration.getColorHeight())};
    vector<optional<int>> uv_index_map{get_index_map(standard_calibration,
                                                     standard_calibration.getColorWidth() / 2,
                                                     standard_calibration.getColorHeight() / 2,
                                                     original_calibration,
                                                     original_calibration.getColorWidth() / 2,
                                                     original_calibration.getColorHeight() / 2)};
    vector<optional<int>> depth_index_map{get_index_map(standard_calibration,
                                                        standard_calibration.getDepthWidth(),
                                                        standard_calibration.getDepthHeight(),
                                                        original_calibration,
                                                        original_calibration.getDepthWidth(),
                                                        original_calibration.getDepthHeight())};

    FileWriter file_writer{output_path, standard_calibration, writer_config};

    ColorEncoder color_encoder{ColorCodecType::VP8,
                               standard_calibration.getColorWidth(),
                               standard_calibration.getColorHeight(),
                               3500,
                               30};
    unique_ptr<DepthEncoder> depth_encoder{DepthEncoder::createTDC1Encoder(
        standard_calibration.getDepthWidth(), standard_calibration.getDepthHeight(), 500)};

    ColorDecoder color_decoder{ColorCodecType::VP8};
    TDC1Decoder depth_decoder;
    bool first{true};
    int audio_frame_index{0};
    int imu_frame_index{0};
    int trs_frame_index{0};
    for (auto& video_frame : video_frames) {
        auto video_global_timecode{video_frame->global_timecode()};
        auto color_frame{color_decoder.decode(video_frame->color_bytes())};
        auto depth_frame{depth_decoder.decode(video_frame->depth_bytes())};

        bool keyframe{video_frame->keyframe()};
        if (first) {
            file_writer.writeCover(*color_frame);
            first = false;
        }

        vector<uint8_t> mapped_y_channel(standard_calibration.getColorWidth() *
                                         standard_calibration.getColorHeight());
        for (size_t i{0}; i < mapped_y_channel.size(); ++i) {
            auto index{y_index_map[i]};
            if (!index) {
                mapped_y_channel[i] = 0;
            } else {
                mapped_y_channel[i] = color_frame->y_channel()[*index];
            }
        }
        vector<uint8_t> mapped_u_channel(standard_calibration.getColorWidth() *
                                         standard_calibration.getColorHeight() / 4);
        vector<uint8_t> mapped_v_channel(standard_calibration.getColorWidth() *
                                         standard_calibration.getColorHeight() / 4);
        for (size_t i{0}; i < mapped_u_channel.size(); ++i) {
            auto index{uv_index_map[i]};
            if (!index) {
                // Black color corresponds to y = 0, u = 128, v = 128.
                mapped_u_channel[i] = 128;
                mapped_v_channel[i] = 128;
            } else {
                mapped_u_channel[i] = color_frame->u_channel()[*index];
                mapped_v_channel[i] = color_frame->v_channel()[*index];
            }
        }
        vector<int> mapped_depth_values(standard_calibration.getDepthWidth() *
                                        standard_calibration.getDepthHeight());
        for (size_t i{0}; i < mapped_depth_values.size(); ++i) {
            auto index{depth_index_map[i]};
            if (!index) {
                mapped_depth_values[i] = 0;
            } else {
//                spdlog::info("index.has_value: {}", index.has_value());
//                spdlog::info("index: {}", *index);
                mapped_depth_values[i] = depth_frame->values()[*index];
            }
        }

        YuvFrame mapped_color_frame{standard_calibration.getColorWidth(),
                                    standard_calibration.getColorHeight(),
                                    std::move(mapped_y_channel),
                                    std::move(mapped_u_channel),
                                    std::move(mapped_v_channel)};

        auto color_bytes{color_encoder.encode(mapped_color_frame, keyframe)};
        auto depth_bytes{depth_encoder->encode(mapped_depth_values, keyframe)};

        file_writer.writeVideoFrame(video_global_timecode,
                                    keyframe,
                                    color_bytes,
                                    depth_bytes);

        while (audio_frame_index < file->audio_frames().size()) {
            auto& audio_frame{file->audio_frames()[audio_frame_index]};
            if (audio_frame->global_timecode() > video_global_timecode)
                break;
            file_writer.writeAudioFrame(*audio_frame);
            ++audio_frame_index;
        }
        while (imu_frame_index < file->imu_frames().size()) {
            auto& imu_frame{file->imu_frames()[imu_frame_index]};
            if (imu_frame->global_timecode() > video_global_timecode)
                break;
            file_writer.writeIMUFrame(*imu_frame);
            ++imu_frame_index;
        }
        while (trs_frame_index < file->trs_frames().size()) {
            auto& trs_frame{file->trs_frames()[trs_frame_index]};
            if (trs_frame->global_timecode() > video_global_timecode)
                break;
            file_writer.writeTRSFrame(*trs_frame);
            ++trs_frame_index;
        }
    }

    file_writer.flush();
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
    root_menu->Insert("trim", [](std::ostream& out) {
        auto video_folder{rgbd::VideoFolder::createFromDefaultPath()};
        auto file_path{video_folder->runSelectFileCLI()};

        std::cout << "From:" << std::endl;
        float from_sec;
        std::cin >> from_sec;
        std::cout << "To:" << std::endl;
        float to_sec;
        std::cin >> to_sec;
        trim_file(file_path->generic_u8string(), from_sec, to_sec);
    });
    root_menu->Insert("standardize", [](std::ostream& out) {
        auto video_folder{rgbd::VideoFolder::createFromDefaultPath()};
        auto file_path{video_folder->runSelectFileCLI()};
        standardize_calibration(file_path->generic_u8string());
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
