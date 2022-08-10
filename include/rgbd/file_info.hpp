#pragma once

#include "constants.hpp"
#include "camera_calibration.hpp"

namespace rgbd
{
struct FileVideoTrack
{
    int track_number;
    string codec;
    int width;
    int height;
};

class FileInfo
{
public:
    FileInfo()
        : writing_app_{""}
        , duration_us_{0.0}
        , color_track_info_{0, "", 0, 0}
        , depth_track_info_{0, "", 0, 0}
        , depth_confidence_track_info_{nullopt}
        , camera_calibration_{nullptr}
    {
    }
    const string& writing_app() const noexcept
    {
        return writing_app_;
    }
    void set_writing_app(const string& writing_app) noexcept
    {
        writing_app_ = writing_app;
    }
    double duration_us() const noexcept
    {
        return duration_us_;
    }
    void set_duration_us(double duration_us) noexcept
    {
        duration_us_ = duration_us;
    }
    const FileVideoTrack& color_track_info() const noexcept
    {
        return color_track_info_;
    }
    void set_color_track_info(const FileVideoTrack& color_track_info) noexcept
    {
        color_track_info_ = color_track_info;
    }
    const FileVideoTrack& depth_track_info() const noexcept
    {
        return depth_track_info_;
    }
    void set_depth_track_info(const FileVideoTrack& depth_track_info) noexcept
    {
        depth_track_info_ = depth_track_info;
    }
    const optional<FileVideoTrack>& depth_confidence_track_info() const noexcept
    {
        return depth_confidence_track_info_;
    }
    void set_depth_confidence_track_info(const FileVideoTrack& depth_track_info) noexcept
    {
        depth_track_info_ = depth_track_info;
    }
    void set_camera_calibration(CameraCalibration* camera_calibration) noexcept
    {
        camera_calibration_ = shared_ptr<CameraCalibration>(camera_calibration);
    }
    const shared_ptr<CameraCalibration> camera_calibration() const noexcept
    {
        return camera_calibration_;
    }
    const Bytes& cover_png_bytes() const noexcept
    {
        return cover_png_bytes_;
    }
    void set_cover_png_bytes(const Bytes& cover_png_bytes) noexcept
    {
        cover_png_bytes_ = cover_png_bytes;
    }

private:
    string writing_app_;
    double duration_us_;
    FileVideoTrack color_track_info_;
    FileVideoTrack depth_track_info_;
    optional<FileVideoTrack> depth_confidence_track_info_;
    shared_ptr<CameraCalibration> camera_calibration_;
    Bytes cover_png_bytes_;
};
}
