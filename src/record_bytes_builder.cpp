#include "record_bytes_builder.hpp"

namespace rgbd
{
RecordBytesBuilder::RecordBytesBuilder()
    : sample_rate_{AUDIO_SAMPLE_RATE}
    , calibration_{}
    , depth_codec_type_{DepthCodecType::TDC1}
    , depth_unit_{DEFAULT_DEPTH_UNIT}
    , cover_png_bytes_{std::nullopt}
    , video_frames_{}
    , audio_frames_{}
    , imu_frames_{}
    , pose_frames_{}
    , calibration_frames_{}
{
}

void RecordBytesBuilder::setSampleRate(int sample_rate)
{
    sample_rate_ = sample_rate;
}

void RecordBytesBuilder::setDepthCodecType(DepthCodecType depth_codec_type)
{
    depth_codec_type_ = depth_codec_type;
}

void RecordBytesBuilder::setDepthUnit(float depth_unit)
{
    depth_unit_ = depth_unit;
}

void RecordBytesBuilder::setCalibration(const CameraCalibration& calibration)
{
    calibration_ = calibration.clone();
}

void RecordBytesBuilder::setCoverPNGBytes(const optional<Bytes>& cover_png_bytes)
{
    cover_png_bytes_ = cover_png_bytes;
}

void RecordBytesBuilder::addVideoFrame(const RecordVideoFrame& video_frame)
{
    video_frames_.push_back(video_frame);
}

void RecordBytesBuilder::addAudioFrame(const RecordAudioFrame& audio_frame)
{
    audio_frames_.push_back(audio_frame);
}

void RecordBytesBuilder::addIMUFrame(const RecordIMUFrame& imu_frame)
{
    imu_frames_.push_back(imu_frame);
}

void RecordBytesBuilder::addPoseFrame(const RecordPoseFrame& pose_frame)
{
    pose_frames_.push_back(pose_frame);
}

void RecordBytesBuilder::addCalibrationFrame(const RecordCalibrationFrame& calibration_frame)
{
    calibration_frames_.push_back(calibration_frame);
}

Bytes RecordBytesBuilder::build()
{
    MemIOCallback io_callback;
    _build(io_callback);

    uint64_t size{io_callback.GetDataBufferSize()};
    Bytes bytes(size);
    memcpy(bytes.data(), io_callback.GetDataBuffer(), size);
    return bytes;
}

void RecordBytesBuilder::buildToPath(const std::string& path)
{
    StdIOCallback io_callback{path.c_str(), MODE_CREATE};
    _build(io_callback);
}

void RecordBytesBuilder::_build(IOCallback& io_callback)
{
    sort(video_frames_.begin(),
         video_frames_.end(),
         [](const RecordVideoFrame& lhs, const RecordVideoFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });
    sort(audio_frames_.begin(),
         audio_frames_.end(),
         [](const RecordAudioFrame& lhs, const RecordAudioFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });
    sort(imu_frames_.begin(),
         imu_frames_.end(),
         [](const RecordIMUFrame& lhs, const RecordIMUFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });
    sort(pose_frames_.begin(),
         pose_frames_.end(),
         [](const RecordPoseFrame& lhs, const RecordPoseFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });
    sort(calibration_frames_.begin(),
         calibration_frames_.end(),
         [](const RecordCalibrationFrame& lhs, const RecordCalibrationFrame& rhs) {
             return lhs.time_point_us() < rhs.time_point_us();
         });

    if (!calibration_) {
        throw std::runtime_error("No CameraCalibration found from RecordBytesBuilder");
    }

    RecordWriter file_writer{io_callback,
                           sample_rate_,
                           depth_codec_type_,
                           depth_unit_,
                           *calibration_,
                           cover_png_bytes_};

    if (video_frames_.size() == 0) {
        spdlog::info("No video frame found from RecordBytesBuilder.");
        file_writer.flush();
        return;
    }

    // Find minimum_time_point_us.
    int64_t initial_video_time_point{video_frames_[0].time_point_us()};

    size_t audio_frame_index{0};
    size_t imu_frame_index{0};
    size_t pose_frame_index{0};
    size_t calibration_frame_index{0};
    for (auto& video_frame : video_frames_) {
        int64_t video_time_point_us{video_frame.time_point_us()};

        while (audio_frame_index < audio_frames_.size()) {
            auto& audio_frame{audio_frames_[audio_frame_index]};
            int64_t audio_time_point_us{audio_frame.time_point_us() - initial_video_time_point};
            // Skip frames before the first video frame.
            if (audio_time_point_us < 0) {
                ++audio_frame_index;
                continue;
            }
            // Write if it is before the current video frame.
            if (audio_frame.time_point_us() > video_time_point_us)
                break;
            file_writer.writeAudioFrame(RecordAudioFrame{audio_time_point_us, audio_frame.bytes()});
            ++audio_frame_index;
        }
        while (imu_frame_index < imu_frames_.size()) {
            auto& imu_frame{imu_frames_[imu_frame_index]};
            int64_t imu_time_point_us{imu_frame.time_point_us() - initial_video_time_point};
            if (imu_time_point_us < 0) {
                ++imu_frame_index;
                continue;
            }
            if (imu_frame.time_point_us() > video_time_point_us)
                break;
            file_writer.writeIMUFrame(RecordIMUFrame{imu_time_point_us,
                                                   imu_frame.acceleration(),
                                                   imu_frame.rotation_rate(),
                                                   imu_frame.magnetic_field(),
                                                   imu_frame.gravity()});
            ++imu_frame_index;
        }
        while (pose_frame_index < pose_frames_.size()) {
            auto& pose_frame{pose_frames_[pose_frame_index]};
            int64_t pose_time_point_us{pose_frame.time_point_us() - initial_video_time_point};
            if (pose_time_point_us < 0) {
                ++pose_frame_index;
                continue;
            }
            if (pose_frame.time_point_us() > video_time_point_us)
                break;
            file_writer.writePoseFrame(RecordPoseFrame{pose_time_point_us,
                                                       pose_frame.translation(),
                                                       pose_frame.rotation()});
            ++pose_frame_index;
        }
        while (calibration_frame_index < calibration_frames_.size()) {
            auto& calibration_frame{calibration_frames_[calibration_frame_index]};
            int64_t calibration_time_point_us{calibration_frame.time_point_us() - initial_video_time_point};
            if (calibration_time_point_us < 0) {
                ++calibration_frame_index;
                continue;
            }
            if (calibration_frame.time_point_us() > video_time_point_us)
                break;
            file_writer.writeCalibrationFrame(RecordCalibrationFrame{calibration_time_point_us,
                                                                     calibration_frame.camera_calibration()});
            ++calibration_frame_index;
        }
        file_writer.writeVideoFrame(video_frame);
    }

    file_writer.flush();
}
} // namespace rgbd
