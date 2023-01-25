#pragma once

#ifdef CMAKE_RGBD_OS_WASM
#include <emscripten.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef CMAKE_RGBD_OS_WINDOWS
#define RGBD_INTERFACE_EXPORT __declspec(dllexport)
#elif CMAKE_RGBD_OS_WASM
#define RGBD_INTERFACE_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define RGBD_INTERFACE_EXPORT
#endif

    //////// START ENUMS ////////
    typedef enum
    {
        RGBD_CAMERA_DEVICE_TYPE_AZURE_KINECT = 0,
        RGBD_CAMERA_DEVICE_TYPE_IOS = 1,
        RGBD_CAMERA_DEVICE_TYPE_UNDISTORTED = 2
    } rgbdCameraDeviceType;

    typedef enum
    {
        RGBD_COLOR_CODEC_TYPE_VP8 = 0
    } rgbdColorCodecType;

    typedef enum
    {
        RGBD_DEPTH_CODEC_TYPE_RVL = 0,
        RGBD_DEPTH_CODEC_TYPE_TDC1 = 1
    } rgbdDepthCodecType;

    typedef enum
    {
        RGBD_FILE_FRAME_TYPE_VIDEO = 0,
        RGBD_FILE_FRAME_TYPE_AUDIO = 1
    } rgbdFileFrameType;
    //////// END ENUMS ////////

    //////// START CONSTANTS ////////
    RGBD_INTERFACE_EXPORT int RGBD_MAJOR_VERSION();
    RGBD_INTERFACE_EXPORT int RGBD_MINOR_VERSION();
    RGBD_INTERFACE_EXPORT int RGBD_PATCH_VERSION();
    RGBD_INTERFACE_EXPORT float RGBD_DEFAULT_DEPTH_UNIT();
    RGBD_INTERFACE_EXPORT int RGBD_AUDIO_SAMPLE_RATE();
    RGBD_INTERFACE_EXPORT int RGBD_AUDIO_INPUT_CHANNEL_COUNT();
    RGBD_INTERFACE_EXPORT int RGBD_AUDIO_INPUT_SAMPLES_PER_FRAME();
    RGBD_INTERFACE_EXPORT int RGBD_AUDIO_OUTPUT_CHANNEL_COUNT();
    RGBD_INTERFACE_EXPORT int RGBD_AUDIO_OUTPUT_INTERVAL_SECONDS_RECIPROCAL();
    RGBD_INTERFACE_EXPORT int RGBD_AUDIO_OUTPUT_SAMPLES_PER_FRAME();
    //////// END CONSTANTS ////////

    //////// START CAPI CONTAINER CLASSES ////////
    RGBD_INTERFACE_EXPORT void rgbd_native_byte_array_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT uint8_t* rgbd_native_byte_array_get_data(void* ptr);
    RGBD_INTERFACE_EXPORT size_t rgbd_native_byte_array_get_size(void* ptr);

    RGBD_INTERFACE_EXPORT void rgbd_native_float_array_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT float* rgbd_native_float_array_get_data(void* ptr);
    RGBD_INTERFACE_EXPORT size_t rgbd_native_float_array_get_size(void* ptr);

    RGBD_INTERFACE_EXPORT void rgbd_native_int32_array_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int32_t* rgbd_native_int32_array_get_data(void* ptr);
    RGBD_INTERFACE_EXPORT size_t rgbd_native_int32_array_get_size(void* ptr);

    RGBD_INTERFACE_EXPORT void rgbd_native_uint8_array_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT uint8_t* rgbd_native_uint8_array_get_data(void* ptr);
    RGBD_INTERFACE_EXPORT size_t rgbd_native_uint8_array_get_size(void* ptr);

    RGBD_INTERFACE_EXPORT void rgbd_native_string_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT const char* rgbd_native_string_get_c_str(void* ptr);
    //////// END CAPI CONTAINER CLASSES ////////

    //////// START HELPER FUNCTIONS FOR WEBASSEBMLY ////////
    // Inspired by Java JNA class PointerByReference.
    // https://github.com/java-native-access/jna/blob/master/src/com/sun/jna/ptr/PointerByReference.java
    RGBD_INTERFACE_EXPORT void** rgbd_pointer_by_reference_ctor();
    RGBD_INTERFACE_EXPORT void rgbd_pointer_by_reference_dtor(void** ref);
    RGBD_INTERFACE_EXPORT void* rgbd_pointer_by_reference_get_value(void** ref);
    //////// END HELPER FUNCTIONS FOR WEBASSEBMLY ////////

    //////// START AUDIO DECODER ////////
    RGBD_INTERFACE_EXPORT void* rgbd_audio_decoder_ctor();
    RGBD_INTERFACE_EXPORT void rgbd_audio_decoder_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void*
    rgbd_audio_decoder_decode(void* ptr, const uint8_t* opus_frame_data, size_t opus_frame_size);
    //////// END AUDIO DECODER ////////

    //////// START AUDIO ENCODER ////////
    RGBD_INTERFACE_EXPORT void* rgbd_audio_encoder_ctor();
    RGBD_INTERFACE_EXPORT void rgbd_audio_encoder_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void*
    rgbd_audio_encoder_encode(void* ptr, const float* pcm_samples, size_t pcm_samples_size);
    RGBD_INTERFACE_EXPORT void* rgbd_audio_encoder_flush(void* ptr);
    //////// END AUDIO ENCODER ////////

    //////// START AUDIO ENCODER FRAME ////////
    RGBD_INTERFACE_EXPORT void rgbd_audio_encoder_frame_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT size_t rgbd_audio_encoder_frame_get_packet_bytes_list_count(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_audio_encoder_frame_get_packet_bytes(void* ptr, size_t index);
    //////// END AUDIO ENCODER FRAME ////////

    //////// START AV PACKET HANDLE ////////
    RGBD_INTERFACE_EXPORT void rgbd_av_packet_handle_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_av_packet_handle_get_data_bytes(void* ptr);
    //////// END AV PACKET HANDLE ////////

    //////// START CAMERA CALIBRATION ////////
    RGBD_INTERFACE_EXPORT void rgbd_camera_calibration_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT rgbdCameraDeviceType
    rgbd_camera_calibration_get_camera_device_type(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_camera_calibration_get_color_width(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_camera_calibration_get_color_height(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_camera_calibration_get_depth_width(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_camera_calibration_get_depth_height(void* ptr);
    RGBD_INTERFACE_EXPORT void*
    rgbd_camera_calibration_get_direction(void* ptr, float uv_u, float uv_v);
    //////// START CAMERA CALIBRATION ////////

    //////// START COLOR DECODER ////////
    RGBD_INTERFACE_EXPORT void* rgbd_color_decoder_ctor(rgbdColorCodecType type);
    RGBD_INTERFACE_EXPORT void rgbd_color_decoder_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_color_decoder_decode(void* ptr,
                                                          const uint8_t* vp8_frame_data,
                                                          size_t vp8_frame_size);
    //////// END COLOR DECODER ////////

    //////// START COLOR ENCODER ////////
    RGBD_INTERFACE_EXPORT void* rgbd_color_encoder_ctor(
        rgbdColorCodecType type, int width, int height, int target_bitrate, int framerate);
    RGBD_INTERFACE_EXPORT void rgbd_color_encoder_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_color_encoder_encode(void* ptr,
                                                          const uint8_t* y_channel,
                                                          const uint8_t* u_channel,
                                                          const uint8_t* v_channel,
                                                          bool keyframe);
    //////// START COLOR ENCODER ////////

    //////// START DEPTH DECODER ////////
    RGBD_INTERFACE_EXPORT void* rgbd_depth_decoder_ctor(rgbdDepthCodecType depth_codec_type);
    RGBD_INTERFACE_EXPORT void rgbd_depth_decoder_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_depth_decoder_decode(void* ptr,
                                                          const uint8_t* depth_bytes_data,
                                                          size_t depth_bytes_size);
    //////// END DEPTH DECODER ////////

    //////// START DEPTH ENCODER ////////
    RGBD_INTERFACE_EXPORT void* rgbd_depth_encoder_create_rvl_encoder(int width, int height);
    RGBD_INTERFACE_EXPORT void*
    rgbd_depth_encoder_create_tdc1_encoder(int width, int height, int depth_diff_multiplier);
    RGBD_INTERFACE_EXPORT void rgbd_depth_encoder_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_depth_encoder_encode(void* ptr, const int32_t* depth_values, bool keyframe);
    //////// END DEPTH DECODER ////////

    //////// START DIRECTION TABLE ////////
    RGBD_INTERFACE_EXPORT void rgbd_direction_table_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_direction_table_get_width(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_direction_table_get_height(void* ptr);
    RGBD_INTERFACE_EXPORT size_t rgbd_direction_table_get_direction_count(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_direction_table_get_direction_x(void* ptr, size_t index);
    RGBD_INTERFACE_EXPORT float rgbd_direction_table_get_direction_y(void* ptr, size_t index);
    RGBD_INTERFACE_EXPORT float rgbd_direction_table_get_direction_z(void* ptr, size_t index);
    //////// END DIRECTION TABLE ////////

    //////// START FILE ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_get_info(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_get_tracks(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_get_attachments(void* ptr);
    RGBD_INTERFACE_EXPORT size_t rgbd_file_get_video_frame_count(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_get_video_frame(void* ptr, size_t index);
    RGBD_INTERFACE_EXPORT size_t rgbd_file_get_audio_frame_count(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_get_audio_frame(void* ptr, size_t index);
    RGBD_INTERFACE_EXPORT size_t rgbd_file_get_imu_frame_count(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_get_imu_frame(void* ptr, size_t index);
    RGBD_INTERFACE_EXPORT size_t rgbd_file_get_trs_frame_count(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_get_trs_frame(void* ptr, size_t index);
    RGBD_INTERFACE_EXPORT bool rgbd_file_has_direction_table(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_get_direction_table(void* ptr);
    //////// END FILE ////////

    //////// START FILE ATTACHMENTS ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_attachments_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_attachments_get_camera_calibration(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_attachments_get_cover_png_bytes(void* ptr);
    //////// END FILE ATTACHMENTS ////////

    //////// START FILE AUDIO FRAME ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_audio_frame_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int64_t rgbd_file_audio_frame_get_time_point_us(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_audio_frame_get_bytes(void* ptr);
    //////// END FILE AUDIO FRAME ////////

    //////// START FILE AUDIO TRACK ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_audio_track_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_file_audio_track_get_track_number(void* ptr);
    RGBD_INTERFACE_EXPORT double rgbd_file_audio_track_get_sampling_frequency(void* ptr);
    //////// END FILE AUDIO TRACK ////////

    //////// START FILE COLOR VIDEO TRACK ////////
    RGBD_INTERFACE_EXPORT rgbdColorCodecType rgbd_file_color_video_track_get_codec(void* ptr);
    //////// END FILE COLOR VIDEO TRACK ////////

    //////// START FILE DEPTH VIDEO TRACK ////////
    RGBD_INTERFACE_EXPORT rgbdDepthCodecType rgbd_file_depth_video_track_get_codec(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_depth_video_track_get_depth_unit(void* ptr);
    //////// END FILE DEPTH VIDEO TRACK ////////

    //////// START FILE FRAME ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_frame_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT rgbdFileFrameType rgbd_file_frame_get_type(void* ptr);
    //////// END FILE FRAME ////////

    //////// START FILE IMU FRAME ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_imu_frame_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int64_t rgbd_file_imu_frame_get_time_point_us(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_acceleration_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_acceleration_y(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_acceleration_z(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_rotation_rate_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_rotation_rate_y(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_rotation_rate_z(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_magnetic_field_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_magnetic_field_y(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_magnetic_field_z(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_gravity_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_gravity_y(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_imu_frame_get_gravity_z(void* ptr);
    //////// END FILE IMU FRAME ////////

    //////// START FILE INFO ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_info_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT uint64_t rgbd_file_info_get_timecode_scale_ns(void* ptr);
    RGBD_INTERFACE_EXPORT double rgbd_file_info_get_duration_us(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_info_get_writing_app(void* ptr);
    //////// END FILE INFO ////////

    //////// START FILE PARSER ////////
    RGBD_INTERFACE_EXPORT int rgbd_file_parser_ctor_from_data(void** parser_ptr_ref, void* data_ptr, size_t data_size);
    RGBD_INTERFACE_EXPORT void* rgbd_file_parser_ctor_from_path(const char* file_path);
    RGBD_INTERFACE_EXPORT void rgbd_file_parser_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_parser_parse(void* ptr, bool with_frames, bool with_directions);
    //////// END FILE PARSER ////////
 
    //////// START FILE TRACKS ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_tracks_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_tracks_get_color_track(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_tracks_get_depth_track(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_tracks_get_audio_track(void* ptr);
    //////// START FILE TRACKS ////////

    //////// START FILE TRS FRAME ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_trs_frame_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int64_t rgbd_file_trs_frame_get_time_point_us(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_translation_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_translation_y(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_translation_z(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_rotation_w(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_rotation_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_rotation_y(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_rotation_z(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_scale_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_scale_y(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_trs_frame_get_scale_z(void* ptr);
    //////// END FILE TRS FRAME ////////

    //////// START FILE VIDEO FRAME ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_video_frame_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int64_t rgbd_file_video_frame_get_time_point_us(void* ptr);
    RGBD_INTERFACE_EXPORT bool rgbd_file_video_frame_get_keyframe(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_video_frame_get_color_bytes(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_video_frame_get_depth_bytes(void* ptr);
    RGBD_INTERFACE_EXPORT bool rgbd_file_video_frame_has_floor(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_video_frame_get_floor_normal_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_video_frame_get_floor_normal_y(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_video_frame_get_floor_normal_z(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_file_video_frame_get_floor_constant(void* ptr);
    //////// END FILE VIDEO FRAME ////////

    //////// START FILE VIDEO TRACK ////////
    RGBD_INTERFACE_EXPORT void rgbd_file_video_track_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_file_video_track_get_track_number(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_file_video_track_get_width(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_file_video_track_get_height(void* ptr);
    //////// START FILE VIDEO TRACK ////////

    //////// START FILE WRITER ////////
    RGBD_INTERFACE_EXPORT void* rgbd_file_writer_ctor_to_path(const char* file_path,
                                                              void* calibration,
                                                              void* config);
    RGBD_INTERFACE_EXPORT void* rgbd_file_writer_ctor_in_memory(void* calibration,
                                                                void* config);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_write_cover(void* ptr,
                                                            int width,
                                                            int height,
                                                            const uint8_t* y_channel,
                                                            const uint8_t* u_channel,
                                                            const uint8_t* v_channel);
    RGBD_INTERFACE_EXPORT void
    rgbd_file_writer_write_video_frame(void* ptr,
                                       int64_t time_point_us,
                                       bool keyframe,
                                       const uint8_t* color_bytes,
                                       size_t color_byte_size,
                                       const uint8_t* depth_bytes,
                                       size_t depth_byte_size);
    RGBD_INTERFACE_EXPORT void
    rgbd_file_writer_write_video_frame_wasm(void* ptr,
                                            int time_point_us,
                                            bool keyframe,
                                            const uint8_t* color_bytes,
                                            size_t color_byte_size,
                                            const uint8_t* depth_bytes,
                                            size_t depth_byte_size);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_write_audio_frame(void* ptr,
                                                                  int64_t time_point_us,
                                                                  const uint8_t* audio_bytes,
                                                                  size_t audio_byte_size);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_write_audio_frame_wasm(void* ptr,
                                                                       int time_point_us,
                                                                       const uint8_t* audio_bytes,
                                                                       size_t audio_byte_size);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_write_imu_frame(void* ptr,
                                                                int64_t time_point_us,
                                                                float acceleration_x,
                                                                float acceleration_y,
                                                                float acceleration_z,
                                                                float rotation_rate_x,
                                                                float rotation_rate_y,
                                                                float rotation_rate_z,
                                                                float magnetic_field_x,
                                                                float magnetic_field_y,
                                                                float magnetic_field_z,
                                                                float gravity_x,
                                                                float gravity_y,
                                                                float gravity_z);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_write_imu_frame_wasm(void* ptr,
                                                                     int time_point_us,
                                                                     float acceleration_x,
                                                                     float acceleration_y,
                                                                     float acceleration_z,
                                                                     float rotation_rate_x,
                                                                     float rotation_rate_y,
                                                                     float rotation_rate_z,
                                                                     float magnetic_field_x,
                                                                     float magnetic_field_y,
                                                                     float magnetic_field_z,
                                                                     float gravity_x,
                                                                     float gravity_y,
                                                                     float gravity_z);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_write_trs_frame(void* ptr,
                                                                int64_t time_point_us,
                                                                float translation_x,
                                                                float translation_y,
                                                                float translation_z,
                                                                float rotation_w,
                                                                float rotation_x,
                                                                float rotation_y,
                                                                float rotation_z,
                                                                float scale_x,
                                                                float scale_y,
                                                                float scale_z);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_write_trs_frame_wasm(void* ptr,
                                                                     int time_point_us,
                                                                     float translation_x,
                                                                     float translation_y,
                                                                     float translation_z,
                                                                     float rotation_w,
                                                                     float rotation_x,
                                                                     float rotation_y,
                                                                     float rotation_z,
                                                                     float scale_x,
                                                                     float scale_y,
                                                                     float scale_z);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_flush(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_file_writer_get_bytes(void* ptr);
    //////// END FILE WRITER ////////

    //////// START FILE WRITER CONFIG ////////
    RGBD_INTERFACE_EXPORT void* rgbd_file_writer_config_ctor();
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_config_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_config_set_framerate(void* ptr, int framerate);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_config_set_samplerate(void* ptr, int samplerate);
    RGBD_INTERFACE_EXPORT void
    rgbd_file_writer_config_set_depth_codec_type(void* ptr, rgbdDepthCodecType depth_codec_type);
    RGBD_INTERFACE_EXPORT float rgbd_file_writer_config_get_depth_unit(void* ptr);
    RGBD_INTERFACE_EXPORT void rgbd_file_writer_config_set_depth_unit(void* ptr, float depth_unit);
    //////// END FILE WRITER CONFIG ////////

    //////// START FRAME MAPPER ////////
    RGBD_INTERFACE_EXPORT void* rgbd_frame_mapper_ctor(void* src_calibration, void* dst_calibration);
    RGBD_INTERFACE_EXPORT void rgbd_frame_mapper_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_frame_mapper_map_color_frame(void* ptr, void* color_frame);
    RGBD_INTERFACE_EXPORT void* rgbd_frame_mapper_map_depth_frame(void* ptr, void* depth_frame);
    //////// END FRAME MAPPER ////////

    //////// START KINECT CAMERA CALIBRATION ////////
    RGBD_INTERFACE_EXPORT void*
    rgbd_kinect_camera_calibration_ctor(int color_width,
                                        int color_height,
                                        int depth_width,
                                        int depth_height,
                                        int resolution_width,
                                        int resolution_height,
                                        float cx,
                                        float cy,
                                        float fx,
                                        float fy,
                                        float k1,
                                        float k2,
                                        float k3,
                                        float k4,
                                        float k5,
                                        float k6,
                                        float codx,
                                        float cody,
                                        float p1,
                                        float p2,
                                        float max_radius_for_projection);
    RGBD_INTERFACE_EXPORT int rgbd_kinect_camera_calibration_get_resolution_width(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_kinect_camera_calibration_get_resolution_height(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_cx(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_cy(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_fx(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_fy(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_k1(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_k2(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_k3(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_k4(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_k5(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_k6(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_codx(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_cody(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_p1(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_kinect_camera_calibration_get_p2(void* ptr);
    RGBD_INTERFACE_EXPORT float
    rgbd_kinect_camera_calibration_get_max_radius_for_projection(void* ptr);
    //////// END KINECT CAMERA CALIBRATION ////////

    //////// START INT32 FRAME ////////
    RGBD_INTERFACE_EXPORT void rgbd_int32_frame_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_int32_frame_get_width(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_int32_frame_get_height(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_int32_frame_get_values(void* ptr);
    //////// END INT16 FRAME ////////

    //////// START IOS CAMERA CALIBRATION ////////
    RGBD_INTERFACE_EXPORT void*
    rgbd_ios_camera_calibration_ctor(int color_width,
                                     int color_height,
                                     int depth_width,
                                     int depth_height,
                                     float fx,
                                     float fy,
                                     float ox,
                                     float oy,
                                     float reference_dimension_width,
                                     float reference_dimension_height,
                                     float lens_distortion_center_x,
                                     float lens_distortion_center_y,
                                     const float* lens_distortion_lookup_table,
                                     size_t lens_distortion_lookup_table_size,
                                     const float* inverse_lens_distortion_lookup_table,
                                     size_t inverse_lens_distortion_lookup_table_size);
    RGBD_INTERFACE_EXPORT float rgbd_ios_camera_calibration_get_fx(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_ios_camera_calibration_get_fy(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_ios_camera_calibration_get_ox(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_ios_camera_calibration_get_oy(void* ptr);
    RGBD_INTERFACE_EXPORT float
    rgbd_ios_camera_calibration_get_reference_dimension_width(void* ptr);
    RGBD_INTERFACE_EXPORT float
    rgbd_ios_camera_calibration_get_reference_dimension_height(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_ios_camera_calibration_get_lens_distortion_center_x(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_ios_camera_calibration_get_lens_distortion_center_y(void* ptr);
    RGBD_INTERFACE_EXPORT void*
    rgbd_ios_camera_calibration_get_lens_distortion_lookup_table(void* ptr);
    RGBD_INTERFACE_EXPORT void*
    rgbd_ios_camera_calibration_get_inverse_lens_distortion_lookup_table(void* ptr);
    //////// END IOS CAMERA CALIBRATION ////////

    //////// START UNDISTORTED CAMERA CALIBRATION ////////
    RGBD_INTERFACE_EXPORT void* rgbd_undistorted_camera_calibration_ctor(int color_width,
                                                                         int color_height,
                                                                         int depth_width,
                                                                         int depth_height,
                                                                         float fx,
                                                                         float fy,
                                                                         float cx,
                                                                         float cy);
    RGBD_INTERFACE_EXPORT float rgbd_undistorted_camera_calibration_get_fx(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_undistorted_camera_calibration_get_fy(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_undistorted_camera_calibration_get_cx(void* ptr);
    RGBD_INTERFACE_EXPORT float rgbd_undistorted_camera_calibration_get_cy(void* ptr);
    //////// END UNDISTORTED CAMERA CALIBRATION ////////

    //////// START YUV FRAME ////////
    RGBD_INTERFACE_EXPORT void* rgbd_yuv_frame_ctor(int width,
                                                    int height,
                                                    const uint8_t* y_channel,
                                                    const uint8_t* u_channel,
                                                    const uint8_t* v_channel);
    RGBD_INTERFACE_EXPORT void rgbd_yuv_frame_dtor(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_yuv_frame_get_y_channel(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_yuv_frame_get_u_channel(void* ptr);
    RGBD_INTERFACE_EXPORT void* rgbd_yuv_frame_get_v_channel(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_yuv_frame_get_width(void* ptr);
    RGBD_INTERFACE_EXPORT int rgbd_yuv_frame_get_height(void* ptr);
    //////// END YUV FRAME ////////
#ifdef __cplusplus
}
#endif
