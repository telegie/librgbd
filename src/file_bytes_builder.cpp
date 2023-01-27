#include "file_bytes_builder.hpp"

namespace rgbd
{
FileBytesBuilder::FileBytesBuilder()
    : calibration_{}
    , depth_codec_type_{DepthCodecType::TDC1}
    , depth_unit_{std::nullopt}
    , cover_{std::nullopt}
    , video_frames_{}
    , audio_frames_{}
    , imu_frames_{}
    , trs_frames_{}
{
}

void FileBytesBuilder::setCalibration(const CameraCalibration& calibration)
{
    calibration_ = calibration.clone();
}

void FileBytesBuilder::setFramerate(int framerate)
{
    framerate_ = framerate;
}

void FileBytesBuilder::setSamplerate(int samplerate)
{
    samplerate_ = samplerate;
}

void FileBytesBuilder::setDepthCodecType(DepthCodecType depth_codec_type)
{
    depth_codec_type_ = depth_codec_type;
}

void FileBytesBuilder::setDepthUnit(float depth_unit)
{
    depth_unit_ = depth_unit;
}

void FileBytesBuilder::setCover(const YuvFrame& cover)
{
    spdlog::info("setCover");
    cover_ = cover;
    spdlog::info("cover_.has_value(): {}", cover_.has_value());
}

void FileBytesBuilder::addVideoFrame(const FileVideoFrame& video_frame)
{
    video_frames_.push_back(video_frame);
}

void FileBytesBuilder::addAudioFrame(const FileAudioFrame& audio_frame)
{
    audio_frames_.push_back(audio_frame);
}

void FileBytesBuilder::addIMUFrame(const FileIMUFrame& imu_frame)
{
    imu_frames_.push_back(imu_frame);
}

void FileBytesBuilder::addTRSFrame(const FileTRSFrame& trs_frame)
{
    trs_frames_.push_back(trs_frame);
}

Bytes FileBytesBuilder::build()
{
    MemIOCallback io_callback;
    _build(io_callback);

    uint64_t size{io_callback.GetDataBufferSize()};
    Bytes bytes(size);
    memcpy(bytes.data(), io_callback.GetDataBuffer(), size);
    return bytes;
}

void FileBytesBuilder::buildToPath(const std::string& path)
{
    StdIOCallback io_callback{path.c_str(), MODE_CREATE};
    _build(io_callback);
}

void FileBytesBuilder::_build(IOCallback& io_callback)
{
    sort(video_frames_.begin(),
         video_frames_.end(),
         [](const FileVideoFrame& lhs, const FileVideoFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });
    sort(audio_frames_.begin(),
         audio_frames_.end(),
         [](const FileAudioFrame& lhs, const FileAudioFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });
    sort(imu_frames_.begin(),
         imu_frames_.end(),
         [](const FileIMUFrame& lhs, const FileIMUFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });
    sort(trs_frames_.begin(),
         trs_frames_.end(),
         [](const FileTRSFrame& lhs, const FileTRSFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });

    // Find minimum_time_point_us.
    std::vector<int64_t> initial_time_points;
    if (video_frames_.size() > 0)
        initial_time_points.push_back(video_frames_[0].time_point_us());
    if (audio_frames_.size() > 0)
        initial_time_points.push_back(audio_frames_[0].time_point_us());
    if (imu_frames_.size() > 0)
        initial_time_points.push_back(imu_frames_[0].time_point_us());
    if (trs_frames_.size() > 0)
        initial_time_points.push_back(trs_frames_[0].time_point_us());

    if (initial_time_points.size() == 0)
        throw std::runtime_error("No frame found from FileWriterHelper");
    int64_t minimum_time_point_us{
        *std::min_element(initial_time_points.begin(), initial_time_points.end())};

    if (!calibration_) {
        throw std::runtime_error("No CameraCalibration found from FileWriterHelper");
    }

    FileWriterConfig writer_config;
    if (framerate_)
        writer_config.framerate = *framerate_;
    if (samplerate_)
        writer_config.samplerate = *samplerate_;
    writer_config.depth_codec_type = depth_codec_type_;
    if (depth_unit_)
        writer_config.depth_unit = *depth_unit_;
    optional<Bytes> cover_png_bytes;
    if (cover_)
        cover_png_bytes = cover_->getMkvCoverSized().getPNGBytes();
    FileWriter file_writer{io_callback, *calibration_, writer_config, cover_png_bytes};

    size_t audio_frame_index{0};
    size_t imu_frame_index{0};
    size_t trs_frame_index{0};
    for (auto& video_frame : video_frames_) {
        int64_t video_time_point_us{video_frame.time_point_us()};

        while (audio_frame_index < audio_frames_.size()) {
            auto& audio_frame{audio_frames_[audio_frame_index]};
            if (audio_frame.time_point_us() > video_time_point_us)
                break;
            file_writer.writeAudioFrame(FileAudioFrame{
                audio_frame.time_point_us() - minimum_time_point_us, audio_frame.bytes()});
            ++audio_frame_index;
        }
        while (imu_frame_index < imu_frames_.size()) {
            auto& imu_frame{imu_frames_[imu_frame_index]};
            if (imu_frame.time_point_us() > video_time_point_us)
                break;
            file_writer.writeIMUFrame(
                FileIMUFrame{imu_frame.time_point_us() - minimum_time_point_us,
                             imu_frame.acceleration(),
                             imu_frame.rotation_rate(),
                             imu_frame.magnetic_field(),
                             imu_frame.gravity()});
            ++imu_frame_index;
        }
        while (trs_frame_index < trs_frames_.size()) {
            auto& trs_frame{trs_frames_[trs_frame_index]};
            if (trs_frame.time_point_us() > video_time_point_us)
                break;
            file_writer.writeTRSFrame(
                FileTRSFrame{trs_frame.time_point_us() - minimum_time_point_us,
                             trs_frame.translation(),
                             trs_frame.rotation(),
                             trs_frame.scale()});
            ++trs_frame_index;
        }

        file_writer.writeVideoFrame(video_frame);
    }

    file_writer.flush();
}
} // namespace rgbd