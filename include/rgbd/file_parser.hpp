#pragma once

#include "file.hpp"
#include "ios_camera_calibration.hpp"
#include "kinect_camera_calibration.hpp"

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
    string codec;
    int width;
    int height;
};

struct FileTracks
{
    FileVideoTrack color_track;
    FileVideoTrack depth_track;
    optional<FileVideoTrack> depth_confidence_track;
    int audio_track_number;
    int floor_track_number;
};

struct FileAttachments
{
    shared_ptr<CameraCalibration> camera_calibration;
    Bytes cover_png_bytes;
};

class FileParser
{
public:
    FileParser(const void* ptr, size_t size);
    FileParser(const string& file_path);

private:
    void parseExceptClusters();
    optional<const FileOffsets> parseOffsets(unique_ptr<libmatroska::KaxSegment>& segment);
    optional<const FileTracks> parseTracks(unique_ptr<libmatroska::KaxTracks>& tracks);
    optional<const FileAttachments>
    parseAttachments(unique_ptr<libmatroska::KaxAttachments>& attachments);
    FileFrame* parseCluster(unique_ptr<libmatroska::KaxCluster>& cluster);

public:
    unique_ptr<File> parseAllClusters();
    const optional<FileInfo>& file_info() const noexcept
    {
        return file_info_;
    }

    const optional<FileTracks>& file_tracks() const noexcept
    {
        return file_tracks_;
    }

    const optional<FileAttachments>& file_attachments() const noexcept
    {
        return file_attachments_;
    }

private:
    unique_ptr<libebml::IOCallback> input_;
    EbmlStream stream_;
    unique_ptr<libmatroska::KaxSegment> kax_segment_;
    optional<FileOffsets> file_offsets_;
    optional<FileInfo> file_info_;
    optional<FileTracks> file_tracks_;
    optional<FileAttachments> file_attachments_;
};
} // namespace rgbd
