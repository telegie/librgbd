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

    int RGBD_AUDIO_SAMPLE_RATE();
    int RGBD_AUDIO_INPUT_CHANNEL_COUNT();
    int RGBD_AUDIO_INPUT_SAMPLES_PER_FRAME();
    int RGBD_AUDIO_OUTPUT_CHANNEL_COUNT();
    int RGBD_AUDIO_OUTPUT_INTERVAL_SECONDS_RECIPROCAL();
    int RGBD_AUDIO_OUTPUT_SAMPLES_PER_FRAME();

    void rgbd_cbyte_array_dtor(void* ptr);
    uint8_t* rgbd_cbyte_array_data(void* ptr);
    size_t rgbd_cbyte_array_size(void* ptr);

    void rgbd_cfloat_array_dtor(void* ptr);
    float* rgbd_cfloat_array_data(void* ptr);
    size_t rgbd_cfloat_array_size(void* ptr);

    void rgbd_cint16_array_dtor(void* ptr);
    int16_t* rgbd_cint16_array_data(void* ptr);
    size_t rgbd_cint16_array_size(void* ptr);

    void rgbd_cuint8_array_dtor(void* ptr);
    uint8_t* rgbd_cuint8_array_data(void* ptr);
    size_t rgbd_cuint8_array_size(void* ptr);

    void rgbd_cstring_dtor(void* ptr);
    const char* rgbd_cstring_c_str(void* ptr);

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
