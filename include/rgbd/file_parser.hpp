#pragma once

#include "file.hpp"
#include "kinect_camera_calibration.hpp"
#include "ios_camera_calibration.hpp"
#include "file_info.hpp"

namespace rgbd
{
class FileParser
{
public:
    FileParser(const void* ptr, std::size_t size);
    FileParser(const string& file_path);

private:
    void init();

public:
    bool hasNextFrame();
    FileFrame* readFrame();
    unique_ptr<File> readAll();
    const FileInfo& info() const noexcept
    {
        return info_;
    }

private:
    unique_ptr<libebml::IOCallback> input_;
    EbmlStream stream_;
    FileInfo info_;
    uint64_t timecode_scale_ns_;
    int color_track_number_;
    int depth_track_number_;
    optional<int> depth_confidence_track_number_;
    int audio_track_number_;
    int floor_track_number_;
    unique_ptr<libmatroska::KaxCluster> cluster_;
};
}
