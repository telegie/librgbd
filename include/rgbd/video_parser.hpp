#pragma once

#include "video.hpp"
#include <rgbd/kinect_camera_calibration.hpp>
#include <rgbd/ios_camera_calibration.hpp>
#include "video_info.hpp"

namespace tg
{

class VideoParser
{
public:
    VideoParser(const void* ptr, std::size_t size);
    VideoParser(const string& file_path);

private:
    void init();

public:
    bool hasNextFrame();
    VideoFrame* readFrame();
    unique_ptr<Video> readAll();
    const VideoInfo& info() const noexcept
    {
        return info_;
    }

private:
    unique_ptr<libebml::IOCallback> input_;
    EbmlStream stream_;
    VideoInfo info_;
    uint64_t timecode_scale_ns_;
    int color_track_number_;
    int depth_track_number_;
    int audio_track_number_;
    int floor_track_number_;
    unique_ptr<libmatroska::KaxCluster> cluster_;
};
}
