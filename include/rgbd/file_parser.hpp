#pragma once

#include "file.hpp"
#include "file_info.hpp"
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
    void init();
    optional<const FileOffsets> parseOffsets(unique_ptr<EbmlElement>& element,
                                             unique_ptr<libmatroska::KaxSegment>& segment);
    optional<const FileTracks> parseTracks(unique_ptr<libmatroska::KaxTracks>& tracks);
    optional<const FileAttachments>
    parseAttachments(unique_ptr<libmatroska::KaxAttachments>& attachments);
    FileFrame* parseCluster(unique_ptr<libmatroska::KaxCluster>& cluster);

public:
    unique_ptr<File> parseAllClusters();
    const FileInfo& info() const noexcept
    {
        return info_;
    }

    const optional<FileAttachments>& file_attachments() const noexcept
    {
        return file_attachments_;
    }

private:
    unique_ptr<libebml::IOCallback> input_;
    EbmlStream stream_;
    FileInfo info_;
    uint64_t timecode_scale_ns_;
    unique_ptr<libmatroska::KaxSegment> segment_;
    optional<FileOffsets> file_offsets_;
    optional<FileTracks> file_tracks_;
    optional<FileAttachments> file_attachments_;
};
} // namespace rgbd
