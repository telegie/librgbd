#pragma once

#include "constants.hpp"
#include "camera_calibration.hpp"

namespace tg
{
class VideoInfo
{
public:
    VideoInfo()
        : writing_app_{""}
        , duration_us_{0.0}
        , color_track_codec_{""}
        , color_track_width_{0}
        , color_track_height_{0}
        , depth_track_codec_{""}
        , depth_track_width_{0}
        , depth_track_height_{0}
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
    const string& color_track_codec() const noexcept
    {
        return color_track_codec_;
    }
    void set_color_track_codec(const string& color_track_codec) noexcept
    {
        color_track_codec_ = color_track_codec;
    }
    int color_track_width() const noexcept
    {
        return color_track_width_;
    }
    void set_color_track_width(int color_track_width) noexcept
    {
        color_track_width_ = color_track_width;
    }
    int color_track_height() const noexcept
    {
        return color_track_height_;
    }
    void set_color_track_height(int color_track_height) noexcept
    {
        color_track_height_ = color_track_height;
    }
    const string& depth_track_codec() const noexcept
    {
        return depth_track_codec_;
    }
    void set_depth_track_codec(const string& depth_track_codec) noexcept
    {
        depth_track_codec_ = depth_track_codec;
    }
    int depth_track_width() const noexcept
    {
        return depth_track_width_;
    }
    void set_depth_track_width(int depth_track_width) noexcept
    {
        depth_track_width_ = depth_track_width;
    }
    int depth_track_height() const noexcept
    {
        return depth_track_height_;
    }
    void set_depth_track_height(int depth_track_height) noexcept
    {
        depth_track_height_ = depth_track_height;
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
    string color_track_codec_;
    int color_track_width_;
    int color_track_height_;
    string depth_track_codec_;
    int depth_track_width_;
    int depth_track_height_;
    shared_ptr<CameraCalibration> camera_calibration_;
    Bytes cover_png_bytes_;
};
}
