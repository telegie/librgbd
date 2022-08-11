#pragma once

#include <random>

#include "audio_frame.hpp"
#include "byte_utils.hpp"
#include "camera_calibration.hpp"
#include "constants.hpp"
#include "ffmpeg_audio_encoder.hpp"
#include "ffmpeg_video_encoder.hpp"
#include "frame.hpp"
#include "ios_camera_calibration.hpp"
#include "kinect_camera_calibration.hpp"
#include "tdc1_encoder.hpp"

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
class FileWriter
{
public:
    FileWriter(const string& file_path,
               bool has_depth_confidence,
               const CameraCalibration& calibration,
               int color_bitrate,
               int framerate,
               int depth_diff_multiplier,
               int samplerate);
    void writeCover(int width,
                    int height,
                    gsl::span<const uint8_t> y_channel,
                    gsl::span<const uint8_t> u_channel,
                    gsl::span<const uint8_t> v_channel);
    void writeVideoFrame(const Frame& rgbd_frame);
    void writeVideoFrame(int64_t time_point_us,
                         int width,
                         int height,
                         gsl::span<const uint8_t> y_channel,
                         gsl::span<const uint8_t> u_channel,
                         gsl::span<const uint8_t> v_channel,
                         gsl::span<const int16_t> depth_values,
                         optional<gsl::span<const uint8_t>> depth_confidence_values,
                         const Plane& floor);
    void writeAudioFrame(const AudioFrame& audio_frame);
    void writeAudioFrame(int64_t time_point_us, gsl::span<const float> pcm_samples);
    void flush();

private:
    std::mt19937 generator_;
    std::uniform_int_distribution<uint64_t> distribution_;
    unique_ptr<StdIOCallback> io_callback_;
    unique_ptr<libmatroska::KaxSegment> segment_;
    libmatroska::KaxTrackEntry* color_track_;
    libmatroska::KaxTrackEntry* depth_track_;
    libmatroska::KaxTrackEntry* depth_confidence_track_;
    libmatroska::KaxTrackEntry* audio_track_;
    libmatroska::KaxTrackEntry* floor_track_;
    unique_ptr<EbmlVoid> seek_head_placeholder_;
    unique_ptr<EbmlVoid> segment_info_placeholder_;
    optional<int64_t> initial_time_point_ns_;
    int64_t last_timecode_;
    int rgbd_index_;
    FFmpegVideoEncoder color_encoder_;
    TDC1Encoder depth_encoder_;
    FFmpegAudioEncoder audio_encoder_;
    int framerate_;
    int samplerate_;
};
} // namespace rgbd
