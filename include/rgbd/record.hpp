#pragma once

#include <glm/gtc/quaternion.hpp>

#pragma warning(push)
#pragma warning(disable : 4245 4267 4828 6387 26495 26812)
#include <ebml/EbmlContexts.h>
#include <ebml/EbmlCrc32.h>
#include <ebml/EbmlHead.h>
#include <ebml/EbmlStream.h>
#include <ebml/EbmlSubHead.h>
#include <ebml/EbmlVoid.h>
#include <ebml/MemReadIOCallback.h>
#include <ebml/StdIOCallback.h>
#include <matroska/FileKax.h>
#include <matroska/KaxBlockData.h>
#include <matroska/KaxCluster.h>
#include <matroska/KaxContexts.h>
#include <matroska/KaxCuesData.h>
#include <matroska/KaxInfoData.h>
#include <matroska/KaxSeekHead.h>
#include <matroska/KaxSegment.h>
#include <matroska/KaxTracks.h>
#pragma warning(pop)

#include "direction_table.hpp"
#include "plane.hpp"

namespace rgbd
{
struct RecordOffsets
{
    int64_t segment_info_offset;
    int64_t tracks_offset;
    int64_t attachments_offset;
    int64_t first_cluster_offset;
};

struct RecordInfo
{
    uint64_t timecode_scale_ns;
    double duration_us;
    string writing_app;
};

struct RecordVideoTrack
{
    int track_number;
    uint64_t default_duration_ns;
    int width;
    int height;
};

struct RecordColorVideoTrack : public RecordVideoTrack
{
    ColorCodecType codec;
};

struct RecordDepthVideoTrack : public RecordVideoTrack
{
    DepthCodecType codec;
    float depth_unit;
};

struct RecordAudioTrack
{
    int track_number;
    double sampling_frequency;
};

struct RecordTracks
{
    RecordColorVideoTrack color_track;
    RecordDepthVideoTrack depth_track;
    RecordAudioTrack audio_track;
    optional<int> floor_track_number;
    optional<int> acceleration_track_number;
    optional<int> rotation_rate_track_number;
    optional<int> magnetic_field_track_number;
    optional<int> gravity_track_number;
    optional<int> translation_track_number;
    optional<int> rotation_track_number;
    optional<int> scale_track_number;
};

struct RecordAttachments
{
    shared_ptr<CameraCalibration> camera_calibration;
    // There may be no cover.png in some early (before v25) files
    optional<Bytes> cover_png_bytes;
};

enum class RecordFrameType
{
    Video = 0,
    Audio = 1,
    IMU = 2,
    Pose = 3
};

class RecordFrame
{
public:
    virtual ~RecordFrame() {}
    virtual RecordFrameType getType() = 0;
};

class RecordVideoFrame : public RecordFrame
{
public:
    RecordVideoFrame(int64_t time_point_us,
                     bool keyframe,
                     const Bytes& color_bytes,
                     const Bytes& depth_bytes)
        : time_point_us_{time_point_us}
        , keyframe_{keyframe}
        , color_bytes_{color_bytes}
        , depth_bytes_{depth_bytes}
    {
    }
    RecordFrameType getType()
    {
        return RecordFrameType::Video;
    }
    int64_t time_point_us() const noexcept
    {
        return time_point_us_;
    }
    bool keyframe() const noexcept
    {
        return keyframe_;
    }
    const Bytes& color_bytes() const noexcept
    {
        return color_bytes_;
    }
    const Bytes& depth_bytes() const noexcept
    {
        return depth_bytes_;
    }

private:
    int64_t time_point_us_;
    bool keyframe_;
    Bytes color_bytes_;
    Bytes depth_bytes_;
};

class RecordAudioFrame : public RecordFrame
{
public:
    RecordAudioFrame(int64_t time_point_us,
                     const Bytes& bytes)
        : time_point_us_{time_point_us}
        , bytes_{bytes}
    {
    }
    RecordFrameType getType()
    {
        return RecordFrameType::Audio;
    }
    int64_t time_point_us() const noexcept
    {
        return time_point_us_;
    }
    const Bytes& bytes() const noexcept
    {
        return bytes_;
    }

private:
    int64_t time_point_us_;
    Bytes bytes_;
};

class RecordIMUFrame : public RecordFrame
{
public:
    RecordIMUFrame(int64_t time_point_us,
                 const glm::vec3& acceleration,
                 const glm::vec3& rotation_rate,
                 const glm::vec3& magnetic_field,
                 const glm::vec3& gravity)
        : time_point_us_{time_point_us}
        , acceleration_{acceleration}
        , rotation_rate_{rotation_rate}
        , magnetic_field_{magnetic_field}
        , gravity_{gravity}
    {
    }
    RecordFrameType getType()
    {
        return RecordFrameType::IMU;
    }
    int64_t time_point_us() const noexcept
    {
        return time_point_us_;
    }
    const glm::vec3& acceleration() const noexcept
    {
        return acceleration_;
    }
    const glm::vec3& rotation_rate() const noexcept
    {
        return rotation_rate_;
    }
    const glm::vec3& magnetic_field() const noexcept
    {
        return magnetic_field_;
    }
    const glm::vec3& gravity() const noexcept
    {
        return gravity_;
    }

private:
    int64_t time_point_us_;
    glm::vec3 acceleration_;
    glm::vec3 rotation_rate_;
    glm::vec3 magnetic_field_;
    glm::vec3 gravity_;
};

class RecordPoseFrame : public RecordFrame
{
public:
    RecordPoseFrame(int64_t time_point_us,
                    const glm::vec3& translation,
                    const glm::quat& rotation)
        : time_point_us_{time_point_us}
        , translation_{translation}
        , rotation_{rotation}
    {
    }
    RecordFrameType getType()
    {
        return RecordFrameType::Pose;
    }
    int64_t time_point_us() const noexcept
    {
        return time_point_us_;
    }
    const glm::vec3& translation() const noexcept
    {
        return translation_;
    }
    const glm::quat& rotation() const noexcept
    {
        return rotation_;
    }

private:
    int64_t time_point_us_;
    glm::vec3 translation_;
    glm::quat rotation_;
};

class Record
{
public:
    Record(const RecordOffsets& offsets,
         const RecordInfo& info,
         const RecordTracks& tracks,
         const RecordAttachments& attachments,
         vector<RecordVideoFrame>&& video_frames,
         vector<RecordAudioFrame>&& audio_frames,
         vector<RecordIMUFrame>&& imu_frames,
         vector<RecordPoseFrame>&& pose_frames,
         optional<DirectionTable>&& direction_table)
        : offsets_{offsets}
        , info_{info}
        , tracks_{tracks}
        , attachments_{attachments}
        , video_frames_{std::move(video_frames)}
        , audio_frames_{std::move(audio_frames)}
        , imu_frames_{std::move(imu_frames)}
        , pose_frames_{std::move(pose_frames)}
        , direction_table_{std::move(direction_table)}
    {
    }
    RecordOffsets& offsets() noexcept
    {
        return offsets_;
    }
    RecordInfo& info() noexcept
    {
        return info_;
    }
    RecordTracks& tracks() noexcept
    {
        return tracks_;
    }
    RecordAttachments& attachments() noexcept
    {
        return attachments_;
    }
    vector<RecordVideoFrame>& video_frames() noexcept
    {
        return video_frames_;
    }
    vector<RecordAudioFrame>& audio_frames() noexcept
    {
        return audio_frames_;
    }
    vector<RecordIMUFrame>& imu_frames() noexcept
    {
        return imu_frames_;
    }
    vector<RecordPoseFrame>& pose_frames() noexcept
    {
        return pose_frames_;
    }
    optional<DirectionTable>& direction_table() noexcept
    {
        return direction_table_;
    }

private:
    RecordOffsets offsets_;
    RecordInfo info_;
    RecordTracks tracks_;
    RecordAttachments attachments_;
    vector<RecordVideoFrame> video_frames_;
    vector<RecordAudioFrame> audio_frames_;
    vector<RecordIMUFrame> imu_frames_;
    vector<RecordPoseFrame> pose_frames_;
    optional<DirectionTable> direction_table_;
};
} // namespace rgbd
