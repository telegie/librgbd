#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdlib.h>
    typedef enum
    {
        RGBD_CAMERA_DEVICE_TYPE_AZURE_KINECT = 0,
        RGBD_CAMERA_DEVICE_TYPE_IOS = 1
    } rgbdCameraDeviceType;

    typedef enum
    {
        RGBD_COLOR_CODEC_TYPE_VP8 = 0
    } rgbdColorCodecType;

    void* rgbd_ffmpeg_video_decoder_ctor(rgbdColorCodecType type);
    void rgbd_ffmpeg_video_decoder_dtor(void* ptr);
    void*
    rgbd_ffmpeg_video_decoder_decode(void* ptr, const uint8_t* vp8_frame_data, size_t vp8_frame_size);

    void rgbd_video_info_dtor(void* ptr);
    void* rgbd_video_info_get_writing_app(void* ptr);
    double rgbd_video_info_get_duration_us(void* ptr);
    void* rgbd_video_info_get_color_track_codec(void* ptr);
    int rgbd_video_info_get_color_track_width(void* ptr);
    int rgbd_video_info_get_color_track_height(void* ptr);
    void* rgbd_video_info_get_depth_track_codec(void* ptr);
    int rgbd_video_info_get_depth_track_width(void* ptr);
    int rgbd_video_info_get_depth_track_height(void* ptr);
    rgbdCameraDeviceType rgbd_video_info_get_camera_device_type(void* ptr);
    void* rgbd_video_info_get_kinect_camera_calibration(void* ptr);
    void* rgbd_video_info_get_ios_camera_calibration(void* ptr);
    void* rgbd_video_info_get_cover_png_bytes(void* ptr);
#ifdef __cplusplus
}
#endif
