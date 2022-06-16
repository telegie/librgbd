#include "rgbd_capi.h"

#include "ffmpeg_video_decoder.hpp"
#include "video_info.hpp"
#include "kinect_camera_calibration.hpp"
#include "ios_camera_calibration.hpp"
#include "capi_utils.hpp"

void* rgbd_ffmpeg_video_decoder_ctor(rgbdColorCodecType type)
{
    return new rgbd::FFmpegVideoDecoder{static_cast<rgbd::ColorCodecType>(type)};
}

void rgbd_ffmpeg_video_decoder_dtor(void* ptr)
{
    delete static_cast<rgbd::FFmpegVideoDecoder*>(ptr);
}

void* rgbd_ffmpeg_video_decoder_decode(void* ptr,
                                       const uint8_t* vp8_frame_data,
                                       size_t vp8_frame_size)
{
    auto yuv_frame{static_cast<rgbd::FFmpegVideoDecoder*>(ptr)->decode(
        {reinterpret_cast<const std::byte*>(vp8_frame_data), vp8_frame_size})};
    return new rgbd::YuvFrame{std::move(yuv_frame)};
}

void rgbd_video_info_dtor(void* ptr)
{
    delete static_cast<rgbd::VideoInfo*>(ptr);
}

void* rgbd_video_info_get_writing_app(void* ptr)
{
    auto video_info{static_cast<rgbd::VideoInfo*>(ptr)};
    return new rgbd::CString{video_info->writing_app()};
}

double rgbd_video_info_get_duration_us(void* ptr)
{
    return static_cast<rgbd::VideoInfo*>(ptr)->duration_us();
}

void* rgbd_video_info_get_color_track_codec(void* ptr)
{
    auto video_info{static_cast<rgbd::VideoInfo*>(ptr)};
    return new rgbd::CString{video_info->color_track_codec()};
}

int rgbd_video_info_get_color_track_width(void* ptr)
{
    return static_cast<rgbd::VideoInfo*>(ptr)->color_track_width();
}

int rgbd_video_info_get_color_track_height(void* ptr)
{
    return static_cast<rgbd::VideoInfo*>(ptr)->color_track_height();
}

void* rgbd_video_info_get_depth_track_codec(void* ptr)
{
    auto video_info{static_cast<rgbd::VideoInfo*>(ptr)};
    return new rgbd::CString{video_info->depth_track_codec()};
}

int rgbd_video_info_get_depth_track_width(void* ptr)
{
    return static_cast<rgbd::VideoInfo*>(ptr)->depth_track_width();
}

int rgbd_video_info_get_depth_track_height(void* ptr)
{
    return static_cast<rgbd::VideoInfo*>(ptr)->depth_track_height();
}

rgbdCameraDeviceType rgbd_video_info_get_camera_device_type(void* ptr)
{
    auto camera_device_type{
        static_cast<rgbd::VideoInfo*>(ptr)->camera_calibration()->getCameraDeviceType()};
    return static_cast<rgbdCameraDeviceType>(camera_device_type);
}

void* rgbd_video_info_get_kinect_camera_calibration(void* ptr)
{
    auto& calibration{static_cast<rgbd::VideoInfo*>(ptr)->camera_calibration()};
    auto kinect_calibration{dynamic_cast<rgbd::KinectCameraCalibration*>(calibration.get())};
    return new rgbd::KinectCameraCalibration(*kinect_calibration);
}

void* rgbd_video_info_get_ios_camera_calibration(void* ptr)
{
    auto& calibration{static_cast<rgbd::VideoInfo*>(ptr)->camera_calibration()};
    auto ios_calibration{dynamic_cast<rgbd::IosCameraCalibration*>(calibration.get())};
    return new rgbd::IosCameraCalibration(*ios_calibration);
}

void* rgbd_video_info_get_cover_png_bytes(void* ptr)
{
    auto video_info{static_cast<rgbd::VideoInfo*>(ptr)};
    return new rgbd::CByteArray{video_info->cover_png_bytes()};
}