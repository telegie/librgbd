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
#include "record.hpp"

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
struct RecordWriterTracks
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
    libmatroska::KaxTrackEntry* calibration_track{nullptr};
};

class RecordWriter
{
public:
    RecordWriter(IOCallback& io_callback,
                 int sample_rate,
                 DepthCodecType depth_codec_type,
                 float depth_unit,
                 const CameraCalibration& calibration,
                 const optional<Bytes>& cover_png_bytes);
    void writeVideoFrame(const RecordVideoFrame& video_frame);
    void writeAudioFrame(const RecordAudioFrame& audio_frame);
    void writeIMUFrame(const RecordIMUFrame& imu_frame);
    void writePoseFrame(const RecordPoseFrame& pose_frame);
    void writeCalibrationFrame(const RecordCalibrationFrame& calibration_frame);
    void flush();

private:
    IOCallback& io_callback_;
    libmatroska::KaxSegment segment_;
    EbmlVoid seek_head_placeholder_;
    EbmlVoid segment_info_placeholder_;
    RecordWriterTracks writer_tracks_;
    libmatroska::KaxBlockBlob* past_color_block_blob_;
    libmatroska::KaxBlockBlob* past_depth_block_blob_;
    int64_t last_timecode_;
};
} // namespace rgbd
