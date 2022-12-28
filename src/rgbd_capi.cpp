#include "rgbd_capi.h"

#include "capi_containers.hpp"
#include "audio_decoder.hpp"
#include "color_decoder.hpp"
#include "file.hpp"
#include "file_parser.hpp"
#include "file_writer.hpp"
#include "integer_frame.hpp"
#include "ios_camera_calibration.hpp"
#include "kinect_camera_calibration.hpp"
#include "tdc1_decoder.hpp"
#include "undistorted_camera_calibration.hpp"

//////// START CONSTANTS ////////
int RGBD_MAJOR_VERSION()
{
    return rgbd::MAJOR_VERSION;
}

int RGBD_MINOR_VERSION()
{
    return rgbd::MINOR_VERSION;
}

int RGBD_PATCH_VERSION()
{
    return rgbd::PATCH_VERSION;
}

float RGBD_DEFAULT_DEPTH_UNIT()
{
    return rgbd::DEFAULT_DEPTH_UNIT;
}

int RGBD_AUDIO_SAMPLE_RATE()
{
    return rgbd::AUDIO_SAMPLE_RATE;
}

int RGBD_AUDIO_INPUT_CHANNEL_COUNT()
{
    return rgbd::AUDIO_INPUT_CHANNEL_COUNT;
}

int RGBD_AUDIO_INPUT_SAMPLES_PER_FRAME()
{
    return rgbd::AUDIO_INPUT_SAMPLES_PER_FRAME;
}

int RGBD_AUDIO_OUTPUT_CHANNEL_COUNT()
{
    return rgbd::AUDIO_OUTPUT_CHANNEL_COUNT;
}

int RGBD_AUDIO_OUTPUT_INTERVAL_SECONDS_RECIPROCAL()
{
    return rgbd::AUDIO_OUTPUT_INTERVAL_SECONDS_RECIPROCAL;
}

int RGBD_AUDIO_OUTPUT_SAMPLES_PER_FRAME()
{
    return rgbd::AUDIO_OUTPUT_SAMPLES_PER_FRAME;
}
//////// END CONSTANTS ////////

//////// START CAPI CONTAINER CLASSES ////////
void rgbd_native_byte_array_dtor(void* ptr)
{
    delete static_cast<rgbd::NativeByteArray*>(ptr);
}

uint8_t* rgbd_native_byte_array_get_data(void* ptr)
{
    return static_cast<rgbd::NativeByteArray*>(ptr)->data();
}

size_t rgbd_native_byte_array_get_size(void* ptr)
{
    return static_cast<rgbd::NativeByteArray*>(ptr)->size();
}

void rgbd_native_float_array_dtor(void* ptr)
{
    delete static_cast<rgbd::NativeFloatArray*>(ptr);
}

float* rgbd_native_float_array_get_data(void* ptr)
{
    return static_cast<rgbd::NativeFloatArray*>(ptr)->data();
}

size_t rgbd_native_float_array_get_size(void* ptr)
{
    return static_cast<rgbd::NativeFloatArray*>(ptr)->size();
}

void rgbd_native_int32_array_dtor(void* ptr)
{
    delete static_cast<rgbd::NativeInt32Array*>(ptr);
}

int32_t* rgbd_native_int32_array_get_data(void* ptr)
{
    return static_cast<rgbd::NativeInt32Array*>(ptr)->data();
}

size_t rgbd_native_int32_array_get_size(void* ptr)
{
    return static_cast<rgbd::NativeInt32Array*>(ptr)->size();
}

void rgbd_native_uint8_array_dtor(void* ptr)
{
    delete static_cast<rgbd::NativeUInt8Array*>(ptr);
}

uint8_t* rgbd_native_uint8_array_get_data(void* ptr)
{
    return static_cast<rgbd::NativeUInt8Array*>(ptr)->data();
}

size_t rgbd_native_uint8_array_get_size(void* ptr)
{
    return static_cast<rgbd::NativeUInt8Array*>(ptr)->size();
}

void rgbd_native_string_dtor(void* ptr)
{
    delete static_cast<rgbd::NativeString*>(ptr);
}

const char* rgbd_native_string_get_c_str(void* ptr)
{
    return static_cast<rgbd::NativeString*>(ptr)->c_str();
}
//////// END CAPI CONTAINER CLASSES ////////

//////// START HELPER FUNCTIONS FOR WEBASSEBMLY ////////
void** rgbd_pointer_by_reference_ctor()
{
    return new void*;
}

void rgbd_pointer_by_reference_dtor(void** ref)
{
    delete ref;
}

void* rgbd_pointer_by_reference_get_value(void** ref)
{
    return *ref;
}
//////// END HELPER FUNCTIONS FOR WEBASSEBMLY ////////

//////// START AUDIO DECODER ////////
void* rgbd_audio_decoder_ctor()
{
    return new rgbd::AudioDecoder;
}

void rgbd_audio_decoder_dtor(void* ptr)
{
    delete static_cast<rgbd::AudioDecoder*>(ptr);
}

void* rgbd_audio_decoder_decode(void* ptr,
                                const uint8_t* opus_frame_data,
                                size_t opus_frame_size)
{
    auto pmc_values{static_cast<rgbd::AudioDecoder*>(ptr)->decode(
        {reinterpret_cast<const std::byte*>(opus_frame_data), opus_frame_size})};
    return new rgbd::NativeFloatArray{std::move(pmc_values)};
}
//////// END AUDIO DECODER ////////

//////// START AUDIO ENCODER ////////
void* rgbd_audio_encoder_ctor()
{
    return new rgbd::AudioEncoder;
}

void rgbd_audio_encoder_dtor(void* ptr)
{
    delete static_cast<rgbd::AudioEncoder*>(ptr);
}

void* rgbd_audio_encoder_encode(void* ptr, const float* pcm_samples, size_t pcm_samples_size)
{
    auto encoder{static_cast<rgbd::AudioEncoder*>(ptr)};
    auto frame{encoder->encode(gsl::span<const float>{pcm_samples, pcm_samples_size})};
    return frame.release();
}

void* rgbd_audio_encoder_flush(void* ptr)
{
    auto encoder{static_cast<rgbd::AudioEncoder*>(ptr)};
    auto frame{encoder->flush()};
    return frame.release();
}
//////// END AUDIO ENCODER ////////

//////// START AUDIO ENCODER FRAME ////////
void rgbd_audio_encoder_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::AudioEncoderFrame*>(ptr);
}

size_t rgbd_audio_encoder_frame_get_packet_bytes_list_count(void* ptr)
{
    auto frame{static_cast<rgbd::AudioEncoderFrame*>(ptr)};
    return frame->packet_bytes_list.size();
}

void* rgbd_audio_encoder_frame_get_packet_bytes(void* ptr, size_t index)
{
    auto frame{static_cast<rgbd::AudioEncoderFrame*>(ptr)};
    return new rgbd::NativeByteArray{frame->packet_bytes_list[index]};
}
//////// END AUDIO ENCODER FRAME ////////

//////// START AV PACKET HANDLE ////////
void rgbd_av_packet_handle_dtor(void* ptr)
{
    delete static_cast<rgbd::AVPacketHandle*>(ptr);
}

void* rgbd_av_packet_handle_get_data_bytes(void* ptr)
{
    auto packet{static_cast<rgbd::AVPacketHandle*>(ptr)};
    return new rgbd::NativeByteArray{packet->getDataBytes()};
}
//////// END AV PACKET HANDLE ////////

//////// START CAMERA CALIBRATION ////////
void rgbd_camera_calibration_dtor(void* ptr)
{
    delete static_cast<const rgbd::CameraCalibration*>(ptr);
}

rgbdCameraDeviceType rgbd_camera_calibration_get_camera_device_type(void* ptr)
{
    auto camera_device_type{static_cast<rgbd::CameraCalibration*>(ptr)->getCameraDeviceType()};
    return static_cast<rgbdCameraDeviceType>(camera_device_type);
}

int rgbd_camera_calibration_get_color_width(void* ptr)
{
    return static_cast<rgbd::CameraCalibration*>(ptr)->getColorWidth();
}

int rgbd_camera_calibration_get_color_height(void* ptr)
{
    return static_cast<rgbd::CameraCalibration*>(ptr)->getColorHeight();
}

int rgbd_camera_calibration_get_depth_width(void* ptr)
{
    return static_cast<rgbd::CameraCalibration*>(ptr)->getDepthWidth();
}

int rgbd_camera_calibration_get_depth_height(void* ptr)
{
    return static_cast<rgbd::CameraCalibration*>(ptr)->getDepthHeight();
}

void* rgbd_camera_calibration_get_direction(void* ptr, float uv_u, float uv_v)
{
    auto camera_calibration{static_cast<rgbd::CameraCalibration*>(ptr)};
    auto direction{camera_calibration->getDirection(glm::vec2{uv_u, uv_v})};
    std::vector<float> values{direction.x, direction.y, direction.z};
    return new rgbd::NativeFloatArray(std::move(values));
}
//////// END CAMERA CALIBRATION ////////

//////// START COLOR DECODER ////////
void* rgbd_color_decoder_ctor(rgbdColorCodecType type)
{
    return new rgbd::ColorDecoder{static_cast<rgbd::ColorCodecType>(type)};
}

void rgbd_color_decoder_dtor(void* ptr)
{
    delete static_cast<rgbd::ColorDecoder*>(ptr);
}

void* rgbd_color_decoder_decode(void* ptr, const uint8_t* vp8_frame_data, size_t vp8_frame_size)
{
    auto yuv_frame{static_cast<rgbd::ColorDecoder*>(ptr)->decode(
        {reinterpret_cast<const std::byte*>(vp8_frame_data), vp8_frame_size})};
    return yuv_frame.release();
}
//////// END COLOR DECODER ////////

//////// START COLOR ENCODER ////////
void* rgbd_color_encoder_ctor(
    rgbdColorCodecType type, int width, int height, int target_bitrate, int framerate)
{
    return new rgbd::ColorEncoder{
        static_cast<rgbd::ColorCodecType>(type), width, height, target_bitrate, framerate};
}

void rgbd_color_encoder_dtor(void* ptr)
{
    delete static_cast<rgbd::ColorEncoder*>(ptr);
}

void* rgbd_color_encoder_encode(void* ptr,
                                const uint8_t* y_channel,
                                const uint8_t* u_channel,
                                const uint8_t* v_channel,
                                bool keyframe)
{
    auto encoder{static_cast<rgbd::ColorEncoder*>(ptr)};
    auto bytes{encoder->encode(y_channel, u_channel, v_channel, keyframe)};
    return new rgbd::NativeByteArray{std::move(bytes)};
}
//////// START COLOR ENCODER ////////

//////// START DEPTH DECODER ////////
void* rgbd_depth_decoder_ctor(rgbdDepthCodecType depth_codec_type)
{
    return new rgbd::DepthDecoder{static_cast<rgbd::DepthCodecType>(depth_codec_type)};
}

void rgbd_depth_decoder_dtor(void* ptr)
{
    delete static_cast<rgbd::DepthDecoder*>(ptr);
}

void* rgbd_depth_decoder_decode(void* ptr,
                                const uint8_t* depth_bytes_data,
                                size_t depth_bytes_size)
{
    auto depth_frame{static_cast<rgbd::DepthDecoder*>(ptr)->decode(
        {reinterpret_cast<const std::byte*>(depth_bytes_data), depth_bytes_size})};
    return depth_frame.release();
}
//////// END DEPTH DECODER ////////

//////// START DEPTH ENCODER ////////
void* rgbd_depth_encoder_create_rvl_encoder(int width, int height)
{
    return rgbd::DepthEncoder::createRVLEncoder(width, height).release();
}

void* rgbd_depth_encoder_create_tdc1_encoder(int width, int height, int depth_diff_multiplier)
{
    return rgbd::DepthEncoder::createTDC1Encoder(width, height, depth_diff_multiplier).release();
}

void rgbd_depth_encoder_dtor(void* ptr)
{
    delete static_cast<rgbd::DepthEncoder*>(ptr);
}

void* rgbd_depth_encoder_encode(void* ptr, const int32_t* depth_values, bool keyframe)
{
    return new rgbd::NativeByteArray{static_cast<rgbd::DepthEncoder*>(ptr)->encode(depth_values, keyframe)};
}
//////// END DEPTH DECODER ////////

//////// START DIRECTION TABLE ////////
void rgbd_direction_table_dtor(void* ptr)
{
    delete static_cast<rgbd::DirectionTable*>(ptr);
}

int rgbd_direction_table_get_width(void* ptr)
{
    return static_cast<rgbd::DirectionTable*>(ptr)->width();
}

int rgbd_direction_table_get_height(void* ptr)
{
    return static_cast<rgbd::DirectionTable*>(ptr)->height();
}

size_t rgbd_direction_table_get_direction_count(void* ptr)
{
    return static_cast<rgbd::DirectionTable*>(ptr)->directions().size();
}

float rgbd_direction_table_get_direction_x(void* ptr, size_t index)
{
    return static_cast<rgbd::DirectionTable*>(ptr)->directions()[index].x;
}

float rgbd_direction_table_get_direction_y(void* ptr, size_t index)
{
    return static_cast<rgbd::DirectionTable*>(ptr)->directions()[index].y;
}

float rgbd_direction_table_get_direction_z(void* ptr, size_t index)
{
    return static_cast<rgbd::DirectionTable*>(ptr)->directions()[index].z;
}
//////// END DIRECTION TABLE ////////

//////// START FILE ////////
void rgbd_file_dtor(void* ptr)
{
    delete static_cast<rgbd::File*>(ptr);
}

void* rgbd_file_get_info(void* ptr)
{
    return &(static_cast<rgbd::File*>(ptr)->info());
}

void* rgbd_file_get_tracks(void* ptr)
{
    return &(static_cast<rgbd::File*>(ptr)->tracks());
}

void* rgbd_file_get_attachments(void* ptr)
{
    return &(static_cast<rgbd::File*>(ptr)->attachments());
}

size_t rgbd_file_get_video_frame_count(void* ptr)
{
    return static_cast<rgbd::File*>(ptr)->video_frames().size();
}

void* rgbd_file_get_video_frame(void* ptr, size_t index)
{
    return static_cast<rgbd::File*>(ptr)->video_frames()[index].get();
}

size_t rgbd_file_get_audio_frame_count(void* ptr)
{
    return static_cast<rgbd::File*>(ptr)->audio_frames().size();
}

void* rgbd_file_get_audio_frame(void* ptr, size_t index)
{
    return static_cast<rgbd::File*>(ptr)->audio_frames()[index].get();
}

size_t rgbd_file_get_imu_frame_count(void* ptr)
{
    return static_cast<rgbd::File*>(ptr)->imu_frames().size();
}

void* rgbd_file_get_imu_frame(void* ptr, size_t index)
{
    return static_cast<rgbd::File*>(ptr)->imu_frames()[index].get();
}

size_t rgbd_file_get_trs_frame_count(void* ptr)
{
    return static_cast<rgbd::File*>(ptr)->trs_frames().size();
}

void* rgbd_file_get_trs_frame(void* ptr, size_t index)
{
    return static_cast<rgbd::File*>(ptr)->trs_frames()[index].get();
}

bool rgbd_file_has_direction_table(void* ptr)
{
    return static_cast<rgbd::File*>(ptr)->direction_table().has_value();
}

void* rgbd_file_get_direction_table(void* ptr)
{
    return &(static_cast<rgbd::File*>(ptr)->direction_table().value());
}
//////// END FILE ////////

//////// START FILE ATTACHMENTS ////////
void rgbd_file_attachments_dtor(void* ptr)
{
    delete static_cast<rgbd::FileAttachments*>(ptr);
}

void* rgbd_file_attachments_get_camera_calibration(void* ptr)
{
    auto file_attachments{static_cast<rgbd::FileAttachments*>(ptr)};
    return file_attachments->camera_calibration.get();
}

void* rgbd_file_attachments_get_cover_png_bytes(void* ptr)
{
    auto file_attachments{static_cast<rgbd::FileAttachments*>(ptr)};
    if (!file_attachments->cover_png_bytes)
        return nullptr;
    return new rgbd::NativeByteArray{*file_attachments->cover_png_bytes};
}
//////// END FILE ATTACHMENTS ////////

//////// START FILE AUDIO FRAME ////////
void rgbd_file_audio_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::FileAudioFrame*>(ptr);
}

int64_t rgbd_file_audio_frame_get_global_timecode(void* ptr)
{
    return static_cast<rgbd::FileAudioFrame*>(ptr)->global_timecode();
}

void* rgbd_file_audio_frame_get_bytes(void* ptr)
{
    return new rgbd::NativeByteArray{static_cast<rgbd::FileAudioFrame*>(ptr)->bytes()};
}
//////// END FILE AUDIO FRAME ////////

//////// START FILE AUDIO TRACK ////////
void rgbd_file_audio_track_dtor(void* ptr)
{
    delete static_cast<rgbd::FileAudioTrack*>(ptr);
}

int rgbd_file_audio_track_get_track_number(void* ptr)
{
    return static_cast<rgbd::FileAudioTrack*>(ptr)->track_number;
}

double rgbd_file_audio_track_get_sampling_frequency(void* ptr)
{
    return static_cast<rgbd::FileAudioTrack*>(ptr)->sampling_frequency;
}
//////// END FILE AUDIO TRACK ////////

//////// START FILE DEPTH VIDEO TRACK ////////
float rgbd_file_depth_video_track_get_depth_unit(void* ptr)
{
    return static_cast<rgbd::FileDepthVideoTrack*>(ptr)->depth_unit;
}
//////// END FILE DEPTH VIDEO TRACK ////////

//////// START FILE FRAME ////////
void rgbd_file_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::FileFrame*>(ptr);
}

rgbdFileFrameType rgbd_file_frame_get_type(void* ptr)
{
    auto type{static_cast<rgbd::FileFrame*>(ptr)->getType()};
    return static_cast<rgbdFileFrameType>(type);
}
//////// END FILE FRAME ////////

//////// START FILE IMU FRAME ////////
void rgbd_file_imu_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::FileIMUFrame*>(ptr);
}

int64_t rgbd_file_imu_frame_get_global_timecode(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->global_timecode();
}

float rgbd_file_imu_frame_get_acceleration_x(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->acceleration().x;
}

float rgbd_file_imu_frame_get_acceleration_y(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->acceleration().y;
}

float rgbd_file_imu_frame_get_acceleration_z(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->acceleration().z;
}

float rgbd_file_imu_frame_get_rotation_rate_x(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->rotation_rate().x;
}

float rgbd_file_imu_frame_get_rotation_rate_y(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->rotation_rate().y;
}

float rgbd_file_imu_frame_get_rotation_rate_z(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->rotation_rate().z;
}

float rgbd_file_imu_frame_get_magnetic_field_x(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->magnetic_field().x;
}

float rgbd_file_imu_frame_get_magnetic_field_y(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->magnetic_field().y;
}

float rgbd_file_imu_frame_get_magnetic_field_z(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->magnetic_field().z;
}
float rgbd_file_imu_frame_get_gravity_x(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->gravity().x;
}

float rgbd_file_imu_frame_get_gravity_y(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->gravity().y;
}

float rgbd_file_imu_frame_get_gravity_z(void* ptr)
{
    return static_cast<rgbd::FileIMUFrame*>(ptr)->gravity().z;
}
//////// END FILE IMU FRAME ////////

//////// START FILE INFO ////////
void rgbd_file_info_dtor(void* ptr)
{
    delete static_cast<rgbd::FileInfo*>(ptr);
}

uint64_t rgbd_file_info_get_timecode_scale_ns(void* ptr)
{
    auto file_info{static_cast<rgbd::FileInfo*>(ptr)};
    return file_info->timecode_scale_ns;
}

double rgbd_file_info_get_duration_us(void* ptr)
{
    auto file_info{static_cast<rgbd::FileInfo*>(ptr)};
    return file_info->duration_us;
}

void* rgbd_file_info_get_writing_app(void* ptr)
{
    auto file_info{static_cast<rgbd::FileInfo*>(ptr)};
    return new rgbd::NativeString{file_info->writing_app};
}
//////// END FILE INFO ////////

//////// START FILE PARSER ////////
int rgbd_file_parser_ctor_from_data(void** parser_ptr_ref, void* data_ptr, size_t data_size)
{
    try {
        *parser_ptr_ref = new rgbd::FileParser{data_ptr, data_size};
        return 0;
    } catch (std::runtime_error e) {
        spdlog::error("error from rgbd_file_parser_ctor_from_data: {}", e.what());
        return -1;
    }
}

void* rgbd_file_parser_ctor_from_path(const char* file_path)
{
    try {
        return new rgbd::FileParser{file_path};
    } catch (std::runtime_error e) {
        spdlog::error("error from rgbd_file_parser_ctor: {}", e.what());
        return nullptr;
    }
}

void rgbd_file_parser_dtor(void* ptr)
{
    delete static_cast<rgbd::FileParser*>(ptr);
}

void* rgbd_file_parser_parse(void* ptr, bool with_frames, bool with_directions)
{
    auto file_parser{static_cast<rgbd::FileParser*>(ptr)};
    return file_parser->parse(with_frames, with_directions).release();
}
//////// END FILE PARSER ////////

//////// START FILE TRACKS ////////
void rgbd_file_tracks_dtor(void* ptr)
{
    delete static_cast<rgbd::FileTracks*>(ptr);
}

void* rgbd_file_tracks_get_color_track(void* ptr)
{
    auto file_tracks{static_cast<rgbd::FileTracks*>(ptr)};
    return &file_tracks->color_track;
}

void* rgbd_file_tracks_get_depth_track(void* ptr)
{
    auto file_tracks{static_cast<rgbd::FileTracks*>(ptr)};
    return &file_tracks->depth_track;
}

void* rgbd_file_tracks_get_audio_track(void* ptr)
{
    auto file_tracks{static_cast<rgbd::FileTracks*>(ptr)};
    return &(file_tracks->audio_track);
}
//////// START FILE TRACKS ////////

//////// START FILE TRS FRAME ////////
void rgbd_file_trs_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::FileTRSFrame*>(ptr);
}

int64_t rgbd_file_trs_frame_get_global_timecode(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->global_timecode();
}

float rgbd_file_trs_frame_get_translation_x(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->translation().x;
}

float rgbd_file_trs_frame_get_translation_y(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->translation().y;
}

float rgbd_file_trs_frame_get_translation_z(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->translation().z;
}

float rgbd_file_trs_frame_get_rotation_w(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->rotation().w;
}

float rgbd_file_trs_frame_get_rotation_x(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->rotation().x;
}

float rgbd_file_trs_frame_get_rotation_y(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->rotation().y;
}

float rgbd_file_trs_frame_get_rotation_z(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->rotation().z;
}

float rgbd_file_trs_frame_get_scale_x(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->scale().x;
}

float rgbd_file_trs_frame_get_scale_y(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->scale().y;
}

float rgbd_file_trs_frame_get_scale_z(void* ptr)
{
    return static_cast<rgbd::FileTRSFrame*>(ptr)->scale().z;
}
//////// END FILE TRS FRAME ////////

//////// START FILE VIDEO FRAME ////////
void rgbd_file_video_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::FileVideoFrame*>(ptr);
}

int64_t rgbd_file_video_frame_get_global_timecode(void* ptr)
{
    return static_cast<rgbd::FileVideoFrame*>(ptr)->global_timecode();
}

bool rgbd_file_video_frame_get_keyframe(void* ptr)
{
    return static_cast<rgbd::FileVideoFrame*>(ptr)->keyframe();
}

void* rgbd_file_video_frame_get_color_bytes(void* ptr)
{
    return new rgbd::NativeByteArray{static_cast<rgbd::FileVideoFrame*>(ptr)->color_bytes()};
}

void* rgbd_file_video_frame_get_depth_bytes(void* ptr)
{
    return new rgbd::NativeByteArray{static_cast<rgbd::FileVideoFrame*>(ptr)->depth_bytes()};
}

bool rgbd_file_video_frame_has_floor(void* ptr)
{
    return static_cast<rgbd::FileVideoFrame*>(ptr)->floor().has_value();
}

float rgbd_file_video_frame_get_floor_normal_x(void* ptr)
{
    auto floor{static_cast<rgbd::FileVideoFrame*>(ptr)->floor()};
    if (!floor)
        return 0.0f;
    return floor->normal().x;
}

float rgbd_file_video_frame_get_floor_normal_y(void* ptr)
{
    auto floor{static_cast<rgbd::FileVideoFrame*>(ptr)->floor()};
    if (!floor)
        return 0.0f;
    return floor->normal().y;
}

float rgbd_file_video_frame_get_floor_normal_z(void* ptr)
{
    auto floor{static_cast<rgbd::FileVideoFrame*>(ptr)->floor()};
    if (!floor)
        return 0.0f;
    return floor->normal().z;
}

float rgbd_file_video_frame_get_floor_constant(void* ptr)
{
    auto floor{static_cast<rgbd::FileVideoFrame*>(ptr)->floor()};
    if (!floor)
        return 0.0f;
    return floor->constant();
}
//////// END FILE VIDEO FRAME ////////

//////// START FILE VIDEO TRACK ////////
void rgbd_file_video_track_dtor(void* ptr)
{
    delete static_cast<rgbd::FileVideoTrack*>(ptr);
}

int rgbd_file_video_track_get_track_number(void* ptr)
{
    auto file_video_track{static_cast<rgbd::FileVideoTrack*>(ptr)};
    return file_video_track->track_number;
}

void* rgbd_file_video_track_get_codec(void* ptr)
{
    auto file_video_track{static_cast<rgbd::FileVideoTrack*>(ptr)};
    return new rgbd::NativeString{file_video_track->codec};
}

int rgbd_file_video_track_get_width(void* ptr)
{
    auto file_video_track{static_cast<rgbd::FileVideoTrack*>(ptr)};
    return file_video_track->width;
}

int rgbd_file_video_track_get_height(void* ptr)
{
    auto file_video_track{static_cast<rgbd::FileVideoTrack*>(ptr)};
    return file_video_track->height;
}
//////// START FILE VIDEO TRACK ////////

//////// START FILE WRITER ////////
void* rgbd_file_writer_ctor_to_path(const char* file_path, void* calibration, void* config)
{
    return new rgbd::FileWriter(file_path,
                                *static_cast<const rgbd::CameraCalibration*>(calibration),
                                *static_cast<const rgbd::FileWriterConfig*>(config));
}

void* rgbd_file_writer_ctor_in_memory(void* calibration, void* config)
{
    return new rgbd::FileWriter(*static_cast<const rgbd::CameraCalibration*>(calibration),
                                *static_cast<const rgbd::FileWriterConfig*>(config));
}

void rgbd_file_writer_dtor(void* ptr)
{
    delete static_cast<rgbd::FileWriter*>(ptr);
}

void rgbd_file_writer_write_cover(void* ptr,
                                  int width,
                                  int height,
                                  const uint8_t* y_channel,
                                  const uint8_t* u_channel,
                                  const uint8_t* v_channel)
{
    static_cast<rgbd::FileWriter*>(ptr)->writeCover(width, height, y_channel, u_channel, v_channel);
}

void rgbd_file_writer_write_video_frame(void* ptr,
                                        int64_t time_point_us,
                                        bool keyframe,
                                        const uint8_t* color_bytes,
                                        size_t color_byte_size,
                                        const uint8_t* depth_bytes,
                                        size_t depth_byte_size)
{
    static_cast<rgbd::FileWriter*>(ptr)->writeVideoFrame(
        time_point_us,
        keyframe,
        gsl::span<const std::byte>{reinterpret_cast<const std::byte*>(color_bytes),
                                   color_byte_size},
        gsl::span<const std::byte>{reinterpret_cast<const std::byte*>(depth_bytes),
                                   depth_byte_size});
}

// int64 is not properly supported by Emscripten yet.
void rgbd_file_writer_write_video_frame_wasm(void* ptr,
                                             int time_point_us,
                                             bool keyframe,
                                             const uint8_t* color_bytes,
                                             size_t color_byte_size,
                                             const uint8_t* depth_bytes,
                                             size_t depth_byte_size)
{
    rgbd_file_writer_write_video_frame(
        ptr, time_point_us, keyframe, color_bytes, color_byte_size, depth_bytes, depth_byte_size);
}

void rgbd_file_writer_write_audio_frame(void* ptr,
                                        int64_t time_point_us,
                                        const uint8_t* audio_bytes,
                                        size_t audio_byte_size)
{
    auto audio_bytes_ptr{reinterpret_cast<const std::byte*> (audio_bytes)};
    const gsl::span<const std::byte> audio_bytes_span{
        reinterpret_cast<const std::byte*>(audio_bytes), audio_byte_size};
    static_cast<rgbd::FileWriter*>(ptr)->writeAudioFrame(time_point_us, audio_bytes_span);
}


void rgbd_file_writer_write_audio_frame_wasm(void* ptr,
                                             int time_point_us,
                                             const uint8_t* audio_bytes,
                                             size_t audio_byte_size)
{
    rgbd_file_writer_write_audio_frame(ptr, time_point_us, audio_bytes, audio_byte_size);
}

void rgbd_file_writer_write_imu_frame(void* ptr,
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
                                      float gravity_z)
{
    auto file_writer{static_cast<rgbd::FileWriter*>(ptr)};
    glm::vec3 acceleration{acceleration_x, acceleration_y, acceleration_z};
    glm::vec3 rotation_rate{rotation_rate_x, rotation_rate_y, rotation_rate_z};
    glm::vec3 magnetic_field{magnetic_field_x, magnetic_field_y, magnetic_field_z};
    glm::vec3 gravity{gravity_x, gravity_y, gravity_z};
    file_writer->writeIMUFrame(time_point_us, acceleration, rotation_rate, magnetic_field, gravity);
}

void rgbd_file_writer_write_imu_frame_wasm(void* ptr,
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
                                           float gravity_z)
{
    rgbd_file_writer_write_imu_frame(ptr, time_point_us,
                                     acceleration_x, acceleration_y, acceleration_z,
                                     rotation_rate_x, rotation_rate_y, rotation_rate_z,
                                     magnetic_field_x, magnetic_field_y, magnetic_field_z,
                                     gravity_x, gravity_y, gravity_z);
}

void rgbd_file_writer_write_trs_frame(void* ptr,
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
                                      float scale_z)
{
    auto file_writer{static_cast<rgbd::FileWriter*>(ptr)};
    glm::vec3 translation{translation_x, translation_y, translation_z};
    glm::quat rotation{rotation_w, rotation_x, rotation_y, rotation_z};
    glm::vec3 scale{scale_x, scale_y, scale_z};
    file_writer->writeTRSFrame(time_point_us, translation, rotation, scale);
}

void rgbd_file_writer_write_trs_frame_wasm(void* ptr,
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
                                           float scale_z)
{
    rgbd_file_writer_write_trs_frame(ptr, time_point_us,
                                     translation_x, translation_y, translation_z,
                                     rotation_w, rotation_x, rotation_y, rotation_z,
                                     scale_x, scale_y, scale_z);
}

void rgbd_file_writer_flush(void* ptr)
{
    static_cast<rgbd::FileWriter*>(ptr)->flush();
}

void* rgbd_file_writer_get_bytes(void* ptr)
{
    auto file_writer{static_cast<rgbd::FileWriter*>(ptr)};
    return new rgbd::NativeByteArray{file_writer->getBytes()};
}
//////// END FILE WRITER ////////

//////// START FILE WRITER CONFIG ////////
void* rgbd_file_writer_config_ctor()
{
    return new rgbd::FileWriterConfig;
}

void rgbd_file_writer_config_dtor(void* ptr)
{
    delete static_cast<rgbd::FileWriterConfig*>(ptr);
}

void rgbd_file_writer_config_set_framerate(void* ptr, int framerate)
{
    static_cast<rgbd::FileWriterConfig*>(ptr)->framerate = framerate;
}

void rgbd_file_writer_config_set_samplerate(void* ptr, int samplerate)
{
    static_cast<rgbd::FileWriterConfig*>(ptr)->samplerate = samplerate;
}

void rgbd_file_writer_config_set_depth_codec_type(void* ptr, rgbdDepthCodecType depth_codec_type)
{
    static_cast<rgbd::FileWriterConfig*>(ptr)->depth_codec_type =
        static_cast<rgbd::DepthCodecType>(depth_codec_type);
}

float rgbd_file_writer_config_get_depth_unit(void* ptr)
{
    return static_cast<rgbd::FileWriterConfig*>(ptr)->depth_unit;
}

void rgbd_file_writer_config_set_depth_unit(void* ptr, float depth_unit)
{
    static_cast<rgbd::FileWriterConfig*>(ptr)->depth_unit = depth_unit;
}
//////// END FILE WRITER CONFIG ////////

//////// START KINECT CAMERA CALIBRATION ////////
void* rgbd_kinect_camera_calibration_ctor(int color_width,
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
                                          float max_radius_for_projection)
{
    return new rgbd::KinectCameraCalibration{color_width,
                                             color_height,
                                             depth_width,
                                             depth_height,
                                             resolution_width,
                                             resolution_height,
                                             cx,
                                             cy,
                                             fx,
                                             fy,
                                             k1,
                                             k2,
                                             k3,
                                             k4,
                                             k5,
                                             k6,
                                             codx,
                                             cody,
                                             p1,
                                             p2,
                                             max_radius_for_projection};
}

int rgbd_kinect_camera_calibration_get_resolution_width(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->resolution_width();
}

int rgbd_kinect_camera_calibration_get_resolution_height(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->resolution_height();
}

float rgbd_kinect_camera_calibration_get_cx(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->cx();
}

float rgbd_kinect_camera_calibration_get_cy(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->cy();
}

float rgbd_kinect_camera_calibration_get_fx(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->fx();
}

float rgbd_kinect_camera_calibration_get_fy(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->fy();
}

float rgbd_kinect_camera_calibration_get_k1(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->k1();
}

float rgbd_kinect_camera_calibration_get_k2(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->k2();
}

float rgbd_kinect_camera_calibration_get_k3(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->k3();
}

float rgbd_kinect_camera_calibration_get_k4(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->k4();
}

float rgbd_kinect_camera_calibration_get_k5(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->k5();
}

float rgbd_kinect_camera_calibration_get_k6(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->k6();
}

float rgbd_kinect_camera_calibration_get_codx(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->codx();
}

float rgbd_kinect_camera_calibration_get_cody(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->cody();
}

float rgbd_kinect_camera_calibration_get_p1(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->p1();
}

float rgbd_kinect_camera_calibration_get_p2(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->p2();
}

float rgbd_kinect_camera_calibration_get_max_radius_for_projection(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->max_radius_for_projection();
}
//////// END KINECT CAMERA CALIBRATION ////////

//////// START INT32 FRAME ////////
void rgbd_int32_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::Int32Frame*>(ptr);
}

int rgbd_int32_frame_get_width(void* ptr)
{
    return static_cast<rgbd::Int32Frame*>(ptr)->width();
}

int rgbd_int32_frame_get_height(void* ptr)
{
    return static_cast<rgbd::Int32Frame*>(ptr)->height();
}

void* rgbd_int32_frame_get_values(void* ptr)
{
    return new rgbd::NativeInt32Array{static_cast<rgbd::Int32Frame*>(ptr)->values()};
}
//////// END INT32 FRAME ////////

//////// START IOS CAMERA CALIBRATION ////////
void* rgbd_ios_camera_calibration_ctor(int color_width,
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
                                       size_t inverse_lens_distortion_lookup_table_size)
{
    return new rgbd::IosCameraCalibration{
        color_width,
        color_height,
        depth_width,
        depth_height,
        fx,
        fy,
        ox,
        oy,
        reference_dimension_width,
        reference_dimension_height,
        lens_distortion_center_x,
        lens_distortion_center_y,
        {lens_distortion_lookup_table, lens_distortion_lookup_table_size},
        {inverse_lens_distortion_lookup_table, inverse_lens_distortion_lookup_table_size}};
}

float rgbd_ios_camera_calibration_get_fx(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->fx();
}

float rgbd_ios_camera_calibration_get_fy(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->fy();
}

float rgbd_ios_camera_calibration_get_ox(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->ox();
}

float rgbd_ios_camera_calibration_get_oy(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->oy();
}

float rgbd_ios_camera_calibration_get_reference_dimension_width(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->reference_dimension_width();
}

float rgbd_ios_camera_calibration_get_reference_dimension_height(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->reference_dimension_height();
}

float rgbd_ios_camera_calibration_get_lens_distortion_center_x(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->lens_distortion_center_x();
}

float rgbd_ios_camera_calibration_get_lens_distortion_center_y(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->lens_distortion_center_y();
}

void* rgbd_ios_camera_calibration_get_lens_distortion_lookup_table(void* ptr)
{
    auto floats{
        static_cast<const rgbd::IosCameraCalibration*>(ptr)->lens_distortion_lookup_table()};
    return new rgbd::NativeFloatArray{std::move(floats)};
}

void* rgbd_ios_camera_calibration_get_inverse_lens_distortion_lookup_table(void* ptr)
{
    auto floats{
        static_cast<const rgbd::IosCameraCalibration*>(ptr)->inverse_lens_distortion_lookup_table()};
    return new rgbd::NativeFloatArray{std::move(floats)};
}
//////// END IOS CAMERA CALIBRATION ////////

//////// START UNDISTORTED CAMERA CALIBRATION ////////
void* rgbd_undistorted_camera_calibration_ctor(int color_width,
                                               int color_height,
                                               int depth_width,
                                               int depth_height,
                                               float fx,
                                               float fy,
                                               float cx,
                                               float cy)
{
    return new rgbd::UndistortedCameraCalibration{
        color_width, color_height, depth_width, depth_height, fx, fy, cx, cy};
}

float rgbd_undistorted_camera_calibration_get_fx(void* ptr)
{
    return static_cast<const rgbd::UndistortedCameraCalibration*>(ptr)->fx();
}

float rgbd_undistorted_camera_calibration_get_fy(void* ptr)
{
    return static_cast<const rgbd::UndistortedCameraCalibration*>(ptr)->fy();
}

float rgbd_undistorted_camera_calibration_get_cx(void* ptr)
{
    return static_cast<const rgbd::UndistortedCameraCalibration*>(ptr)->cx();
}

float rgbd_undistorted_camera_calibration_get_cy(void* ptr)
{
    return static_cast<const rgbd::UndistortedCameraCalibration*>(ptr)->cy();
}
//////// END UNDISTORTED CAMERA CALIBRATION ////////

//////// START YUV FRAME ////////
void rgbd_yuv_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::YuvFrame*>(ptr);
}

void* rgbd_yuv_frame_get_y_channel(void* ptr)
{
    return new rgbd::NativeUInt8Array{static_cast<rgbd::YuvFrame*>(ptr)->y_channel()};
}

void* rgbd_yuv_frame_get_u_channel(void* ptr)
{
    return new rgbd::NativeUInt8Array{static_cast<rgbd::YuvFrame*>(ptr)->u_channel()};
}

void* rgbd_yuv_frame_get_v_channel(void* ptr)
{
    return new rgbd::NativeUInt8Array{static_cast<rgbd::YuvFrame*>(ptr)->v_channel()};
}

int rgbd_yuv_frame_get_width(void* ptr)
{
    return static_cast<rgbd::YuvFrame*>(ptr)->width();
}

int rgbd_yuv_frame_get_height(void* ptr)
{
    return static_cast<rgbd::YuvFrame*>(ptr)->height();
}
//////// END YUV FRAME ////////
