#pragma once

#include <random>
#include <glm/gtc/quaternion.hpp>

#include "audio_frame.hpp"
#include "byte_utils.hpp"
#include "camera_calibration.hpp"
#include "constants.hpp"
#include "audio_encoder.hpp"
#include "color_encoder.hpp"
#include "ios_camera_calibration.hpp"
#include "kinect_camera_calibration.hpp"
#include "video_frame.hpp"
#include "tdc1_encoder.hpp"
#include "file.hpp"

#pragma warning(push)
#pragma warning(disable : 4245 4267 4828 6387 26495 26812)
#include <ebml/EbmlHead.h>
#include <ebml/EbmlSubHead.h>
#include <ebml/EbmlVoid.h>
#include <ebml/MemIOCallback.h>
#include <ebml/StdIOCallback.h>
#include <matroska/FileKax.h>
#include <matroska/KaxCluster.h>
#include <matroska/KaxCues.h>
#include <matroska/KaxInfoData.h>
#include <matroska/KaxSeekHead.h>
#include <matroska/KaxSegment.h>
#include <matroska/KaxTracks.h>
#pragma warning(pop)

namespace rgbd
{
struct FileWriterConfig
{
    int framerate{30};
    int samplerate{AUDIO_SAMPLE_RATE};
    DepthCodecType depth_codec_type{DepthCodecType::TDC1};
    float depth_unit{DEFAULT_DEPTH_UNIT}; // 1 mm
};

struct FileWriterTracks
{
    libmatroska::KaxTrackEntry* color_track{nullptr};
    libmatroska::KaxTrackEntry* depth_track{nullptr};
    libmatroska::KaxTrackEntry* audio_track{nullptr};
    libmatroska::KaxTrackEntry* acceleration_track{nullptr};
    libmatroska::KaxTrackEntry* rotation_rate_track{nullptr};
    libmatroska::KaxTrackEntry* magnetic_field_track{nullptr};
    libmatroska::KaxTrackEntry* gravity_track{nullptr};
    libmatroska::KaxTrackEntry* translation_track{nullptr};
    libmatroska::KaxTrackEntry* rotation_track{nullptr};
    libmatroska::KaxTrackEntry* scale_track{nullptr};
};

class FileWriter
{
public:
    FileWriter(const string& file_path,
               const CameraCalibration& calibration,
               const FileWriterConfig& config);
    FileWriter(const CameraCalibration& calibration,
               const FileWriterConfig& config);
private:
    void init(const CameraCalibration& calibration,
              const FileWriterConfig& config);
public:
    void writeCover(const YuvFrame& yuv_frame);
    void writeCover(int width,
                    int height,
                    gsl::span<const uint8_t> y_channel,
                    gsl::span<const uint8_t> u_channel,
                    gsl::span<const uint8_t> v_channel);
    void writeVideoFrame(int64_t time_point_us,
                         bool keyframe,
                         gsl::span<const byte> color_bytes,
                         gsl::span<const byte> depth_bytes);
    void writeAudioFrame(int64_t time_point_us, gsl::span<const std::byte> frame_data_bytes);
    void writeAudioFrame(const FileAudioFrame& audio_frame);
    void writeIMUFrame(int64_t time_point_us,
                       const glm::vec3& acceleration,
                       const glm::vec3& rotation_rate,
                       const glm::vec3& magnetic_field,
                       const glm::vec3& gravity);
    void writeIMUFrame(const FileIMUFrame& imu_frame);
    void writeTRSFrame(int64_t time_point_us,
                       const glm::vec3& translation,
                       const glm::quat& rotation,
                       const glm::vec3& scale);
    void writeTRSFrame(const FileTRSFrame& trs_frame);
    void flush();
    Bytes getBytes();

private:
    std::mt19937 generator_;
    std::uniform_int_distribution<uint64_t> distribution_;
    unique_ptr<IOCallback> io_callback_;
    unique_ptr<libmatroska::KaxSegment> segment_;
    FileWriterTracks writer_tracks_;
    unique_ptr<EbmlVoid> seek_head_placeholder_;
    unique_ptr<EbmlVoid> segment_info_placeholder_;
    optional<int64_t> initial_time_point_ns_;
    libmatroska::KaxBlockBlob* past_color_block_blob_;
    libmatroska::KaxBlockBlob* past_depth_block_blob_;
    int64_t last_timecode_;
};
} // namespace rgbd
