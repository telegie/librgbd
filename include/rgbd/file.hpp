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
struct FileOffsets
{
    int64_t segment_info_offset;
    int64_t tracks_offset;
    int64_t attachments_offset;
    int64_t first_cluster_offset;
};

struct FileInfo
{
    uint64_t timecode_scale_ns;
    double duration_us;
    string writing_app;
};

struct FileVideoTrack
{
    int track_number;
    uint64_t default_duration_ns;
    int width;
    int height;
};

struct FileColorVideoTrack : public FileVideoTrack
{
    ColorCodecType codec;
};

struct FileDepthVideoTrack : public FileVideoTrack
{
    DepthCodecType codec;
    float depth_unit;
};

struct FileAudioTrack
{
    int track_number;
    double sampling_frequency;
};

struct FileTracks
{
    FileColorVideoTrack color_track;
    FileDepthVideoTrack depth_track;
    FileAudioTrack audio_track;
    optional<int> floor_track_number;
    optional<int> acceleration_track_number;
    optional<int> rotation_rate_track_number;
    optional<int> magnetic_field_track_number;
    optional<int> gravity_track_number;
    optional<int> translation_track_number;
    optional<int> rotation_track_number;
    optional<int> scale_track_number;
};

struct FileAttachments
{
    shared_ptr<CameraCalibration> camera_calibration;
    // There may be no cover.png in some early (before v25) files
    optional<Bytes> cover_png_bytes;
};

enum class FileFrameType
{
    Video = 0,
    Audio = 1,
    IMU = 2,
    TRS = 3
};

class FileFrame
{
public:
    virtual ~FileFrame() {}
    virtual FileFrameType getType() = 0;
};

class FileVideoFrame : public FileFrame
{
public:
    FileVideoFrame(int64_t time_point_us,
                   bool keyframe,
                   const Bytes& color_bytes,
                   const Bytes& depth_bytes)
        : time_point_us_{time_point_us}
        , keyframe_{keyframe}
        , color_bytes_{color_bytes}
        , depth_bytes_{depth_bytes}
    {
    }
    FileFrameType getType()
    {
        return FileFrameType::Video;
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

class FileAudioFrame : public FileFrame
{
public:
    FileAudioFrame(int64_t time_point_us,
                   const Bytes& bytes)
        : time_point_us_{time_point_us}
        , bytes_{bytes}
    {
    }
    FileFrameType getType()
    {
        return FileFrameType::Audio;
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

class FileIMUFrame : public FileFrame
{
public:
    FileIMUFrame(int64_t time_point_us,
                 glm::vec3 acceleration,
                 glm::vec3 rotation_rate,
                 glm::vec3 magnetic_field,
                 glm::vec3 gravity)
        : time_point_us_{time_point_us}
        , acceleration_{acceleration}
        , rotation_rate_{rotation_rate}
        , magnetic_field_{magnetic_field}
        , gravity_{gravity}
    {
    }
    FileFrameType getType()
    {
        return FileFrameType::IMU;
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

class FileTRSFrame : public FileFrame
{
public:
    FileTRSFrame(int64_t time_point_us,
                 const glm::vec3& translation,
                 const glm::quat& rotation,
                 const glm::vec3& scale)
        : time_point_us_{time_point_us}
        , translation_{translation}
        , rotation_{rotation}
        , scale_{scale}
    {
    }
    FileFrameType getType()
    {
        return FileFrameType::TRS;
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
    const glm::vec3& scale() const noexcept
    {
        return scale_;
    }

private:
    int64_t time_point_us_;
    glm::vec3 translation_;
    glm::quat rotation_;
    glm::vec3 scale_;
};

class File
{
public:
    File(const FileOffsets& offsets,
         const FileInfo& info,
         const FileTracks& tracks,
         const FileAttachments& attachments,
         vector<unique_ptr<FileVideoFrame>>&& video_frames,
         vector<unique_ptr<FileAudioFrame>>&& audio_frames,
         vector<unique_ptr<FileIMUFrame>>&& imu_frames,
         vector<unique_ptr<FileTRSFrame>>&& trs_frames,
         optional<DirectionTable>&& direction_table)
        : offsets_{offsets}
        , info_{info}
        , tracks_{tracks}
        , attachments_{attachments}
        , video_frames_{std::move(video_frames)}
        , audio_frames_{std::move(audio_frames)}
        , imu_frames_{std::move(imu_frames)}
        , trs_frames_{std::move(trs_frames)}
        , direction_table_{std::move(direction_table)}
    {
    }
    FileOffsets& offsets() noexcept
    {
        return offsets_;
    }
    FileInfo& info() noexcept
    {
        return info_;
    }
    FileTracks& tracks() noexcept
    {
        return tracks_;
    }
    FileAttachments& attachments() noexcept
    {
        return attachments_;
    }
    const vector<unique_ptr<FileVideoFrame>>& video_frames() const noexcept
    {
        return video_frames_;
    }
    const vector<unique_ptr<FileAudioFrame>>& audio_frames() const noexcept
    {
        return audio_frames_;
    }
    const vector<unique_ptr<FileIMUFrame>>& imu_frames() const noexcept
    {
        return imu_frames_;
    }
    const vector<unique_ptr<FileTRSFrame>>& trs_frames() const noexcept
    {
        return trs_frames_;
    }
    optional<DirectionTable>& direction_table() noexcept
    {
        return direction_table_;
    }

private:
    FileOffsets offsets_;
    FileInfo info_;
    FileTracks tracks_;
    FileAttachments attachments_;
    vector<unique_ptr<FileVideoFrame>> video_frames_;
    vector<unique_ptr<FileAudioFrame>> audio_frames_;
    vector<unique_ptr<FileIMUFrame>> imu_frames_;
    vector<unique_ptr<FileTRSFrame>> trs_frames_;
    optional<DirectionTable> direction_table_;
};
} // namespace rgbd
