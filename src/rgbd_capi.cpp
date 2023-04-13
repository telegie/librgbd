#include "rgbd_capi.h"

#include "rgbd.hpp"

using namespace rgbd;

//////// START CONSTANTS ////////
int RGBD_MAJOR_VERSION()
{
    return MAJOR_VERSION;
}

int RGBD_MINOR_VERSION()
{
    return MINOR_VERSION;
}

int RGBD_PATCH_VERSION()
{
    return PATCH_VERSION;
}

int RGBD_VIDEO_FRAME_RATE()
{
    return VIDEO_FRAME_RATE;
}

float RGBD_DEFAULT_DEPTH_UNIT()
{
    return DEFAULT_DEPTH_UNIT;
}

int RGBD_AUDIO_SAMPLE_RATE()
{
    return AUDIO_SAMPLE_RATE;
}

int RGBD_AUDIO_INPUT_CHANNEL_COUNT()
{
    return AUDIO_INPUT_CHANNEL_COUNT;
}

int RGBD_AUDIO_INPUT_SAMPLES_PER_FRAME()
{
    return AUDIO_INPUT_SAMPLES_PER_FRAME;
}

int RGBD_AUDIO_OUTPUT_CHANNEL_COUNT()
{
    return AUDIO_OUTPUT_CHANNEL_COUNT;
}

int RGBD_AUDIO_OUTPUT_INTERVAL_SECONDS_RECIPROCAL()
{
    return AUDIO_OUTPUT_INTERVAL_SECONDS_RECIPROCAL;
}

int RGBD_AUDIO_OUTPUT_SAMPLES_PER_FRAME()
{
    return AUDIO_OUTPUT_SAMPLES_PER_FRAME;
}
//////// END CONSTANTS ////////

//////// START CAPI CONTAINER CLASSES ////////
void rgbd_native_byte_array_dtor(void* ptr)
{
    delete static_cast<NativeByteArray*>(ptr);
}

uint8_t* rgbd_native_byte_array_get_data(void* ptr)
{
    return reinterpret_cast<uint8_t*>(static_cast<NativeByteArray*>(ptr)->data());
}

size_t rgbd_native_byte_array_get_size(void* ptr)
{
    return static_cast<NativeByteArray*>(ptr)->size();
}

void rgbd_native_float_array_dtor(void* ptr)
{
    delete static_cast<NativeFloatArray*>(ptr);
}

float* rgbd_native_float_array_get_data(void* ptr)
{
    return static_cast<NativeFloatArray*>(ptr)->data();
}

size_t rgbd_native_float_array_get_size(void* ptr)
{
    return static_cast<NativeFloatArray*>(ptr)->size();
}

void rgbd_native_int32_array_dtor(void* ptr)
{
    delete static_cast<NativeInt32Array*>(ptr);
}

int32_t* rgbd_native_int32_array_get_data(void* ptr)
{
    return static_cast<NativeInt32Array*>(ptr)->data();
}

size_t rgbd_native_int32_array_get_size(void* ptr)
{
    return static_cast<NativeInt32Array*>(ptr)->size();
}

void* rgbd_native_quaternion_ctor(float w, float x, float y, float z)
{
    return new NativeQuaternion{w, x, y, z};
}

void rgbd_native_quaternion_dtor(void* ptr)
{
    delete static_cast<NativeQuaternion*>(ptr);
}

float rgbd_native_quaternion_get_w(void* ptr)
{
    return static_cast<NativeQuaternion*>(ptr)->w;
}

float rgbd_native_quaternion_get_x(void* ptr)
{
    return static_cast<NativeQuaternion*>(ptr)->x;
}

float rgbd_native_quaternion_get_y(void* ptr)
{
    return static_cast<NativeQuaternion*>(ptr)->y;
}

float rgbd_native_quaternion_get_z(void* ptr)
{
    return static_cast<NativeQuaternion*>(ptr)->z;
}

void rgbd_native_uint8_array_dtor(void* ptr)
{
    delete static_cast<NativeUInt8Array*>(ptr);
}

uint8_t* rgbd_native_uint8_array_get_data(void* ptr)
{
    return static_cast<NativeUInt8Array*>(ptr)->data();
}

size_t rgbd_native_uint8_array_get_size(void* ptr)
{
    return static_cast<NativeUInt8Array*>(ptr)->size();
}

void rgbd_native_string_dtor(void* ptr)
{
    delete static_cast<NativeString*>(ptr);
}

const char* rgbd_native_string_get_c_str(void* ptr)
{
    return static_cast<NativeString*>(ptr)->c_str();
}

void* rgbd_native_vector3_ctor(float x, float y, float z)
{
    return new NativeVector3{x, y, z};
}

void rgbd_native_vector3_dtor(void* ptr)
{
    delete static_cast<NativeVector3*>(ptr);
}

float rgbd_native_vector3_get_x(void* ptr)
{
    return static_cast<NativeVector3*>(ptr)->x;
}

float rgbd_native_vector3_get_y(void* ptr)
{
    return static_cast<NativeVector3*>(ptr)->y;
}

float rgbd_native_vector3_get_z(void* ptr)
{
    return static_cast<NativeVector3*>(ptr)->z;
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
    return new AudioDecoder;
}

void rgbd_audio_decoder_dtor(void* ptr)
{
    delete static_cast<AudioDecoder*>(ptr);
}

void* rgbd_audio_decoder_decode(void* ptr, const uint8_t* opus_frame_data, size_t opus_frame_size)
{
    auto pmc_values{static_cast<AudioDecoder*>(ptr)->decode(
        {opus_frame_data, opus_frame_size})};
    return new NativeFloatArray{std::move(pmc_values)};
}
//////// END AUDIO DECODER ////////

//////// START AUDIO ENCODER ////////
void* rgbd_audio_encoder_ctor()
{
    return new AudioEncoder;
}

void rgbd_audio_encoder_dtor(void* ptr)
{
    delete static_cast<AudioEncoder*>(ptr);
}

void* rgbd_audio_encoder_encode(void* ptr, const float* pcm_samples, size_t pcm_samples_size)
{
    auto encoder{static_cast<AudioEncoder*>(ptr)};
    auto frame{encoder->encode(span<const float>{pcm_samples, pcm_samples_size})};
    return frame.release();
}

void* rgbd_audio_encoder_flush(void* ptr)
{
    auto encoder{static_cast<AudioEncoder*>(ptr)};
    auto frame{encoder->flush()};
    return frame.release();
}
//////// END AUDIO ENCODER ////////

//////// START AUDIO ENCODER FRAME ////////
void rgbd_audio_encoder_frame_dtor(void* ptr)
{
    delete static_cast<AudioEncoderFrame*>(ptr);
}

size_t rgbd_audio_encoder_frame_get_packet_bytes_list_count(void* ptr)
{
    auto frame{static_cast<AudioEncoderFrame*>(ptr)};
    return frame->packet_bytes_list.size();
}

void* rgbd_audio_encoder_frame_get_packet_bytes(void* ptr, size_t index)
{
    auto frame{static_cast<AudioEncoderFrame*>(ptr)};
    return new NativeByteArray{frame->packet_bytes_list[index]};
}
//////// END AUDIO ENCODER FRAME ////////

//////// START AV PACKET HANDLE ////////
void rgbd_av_packet_handle_dtor(void* ptr)
{
    delete static_cast<AVPacketHandle*>(ptr);
}

void* rgbd_av_packet_handle_get_data_bytes(void* ptr)
{
    auto packet{static_cast<AVPacketHandle*>(ptr)};
    return new NativeByteArray{packet->getDataBytes()};
}
//////// END AV PACKET HANDLE ////////

//////// START CAMERA CALIBRATION ////////
void rgbd_camera_calibration_dtor(void* ptr)
{
    delete static_cast<const CameraCalibration*>(ptr);
}

rgbdCameraCalibrationType rgbd_camera_calibration_get_camera_device_type(void* ptr)
{
    auto camera_device_type{static_cast<CameraCalibration*>(ptr)->getCameraCalibrationType()};
    return static_cast<rgbdCameraCalibrationType>(camera_device_type);
}

int rgbd_camera_calibration_get_color_width(void* ptr)
{
    return static_cast<CameraCalibration*>(ptr)->getColorWidth();
}

int rgbd_camera_calibration_get_color_height(void* ptr)
{
    return static_cast<CameraCalibration*>(ptr)->getColorHeight();
}

int rgbd_camera_calibration_get_depth_width(void* ptr)
{
    return static_cast<CameraCalibration*>(ptr)->getDepthWidth();
}

int rgbd_camera_calibration_get_depth_height(void* ptr)
{
    return static_cast<CameraCalibration*>(ptr)->getDepthHeight();
}

void* rgbd_camera_calibration_get_direction(void* ptr, float uv_u, float uv_v)
{
    auto camera_calibration{static_cast<CameraCalibration*>(ptr)};
    auto direction{camera_calibration->getDirection(glm::vec2{uv_u, uv_v})};
    vector<float> values{direction.x, direction.y, direction.z};
    return new NativeFloatArray(std::move(values));
}
//////// END CAMERA CALIBRATION ////////

//////// START COLOR DECODER ////////
void* rgbd_color_decoder_ctor(rgbdColorCodecType type)
{
    return new ColorDecoder{static_cast<ColorCodecType>(type)};
}

void rgbd_color_decoder_dtor(void* ptr)
{
    delete static_cast<ColorDecoder*>(ptr);
}

void* rgbd_color_decoder_decode(void* ptr, const uint8_t* vp8_frame_data, size_t vp8_frame_size)
{
    auto yuv_frame{static_cast<ColorDecoder*>(ptr)->decode({vp8_frame_data, vp8_frame_size})};
    return yuv_frame.release();
}
//////// END COLOR DECODER ////////

//////// START COLOR ENCODER ////////
void* rgbd_color_encoder_ctor(rgbdColorCodecType type, int width, int height)
{
    return new ColorEncoder{static_cast<ColorCodecType>(type), width, height};
}

void rgbd_color_encoder_dtor(void* ptr)
{
    delete static_cast<ColorEncoder*>(ptr);
}

void* rgbd_color_encoder_encode(void* ptr, void* yuv_frame_ptr, bool keyframe)
{
    auto encoder{static_cast<ColorEncoder*>(ptr)};
    auto bytes{encoder->encode(*static_cast<YuvFrame*>(yuv_frame_ptr), keyframe)};
    return new NativeByteArray{std::move(bytes)};
}
//////// START COLOR ENCODER ////////

//////// START DEPTH DECODER ////////
void* rgbd_depth_decoder_ctor(rgbdDepthCodecType depth_codec_type)
{
    return new DepthDecoder{static_cast<DepthCodecType>(depth_codec_type)};
}

void rgbd_depth_decoder_dtor(void* ptr)
{
    delete static_cast<DepthDecoder*>(ptr);
}

void* rgbd_depth_decoder_decode(void* ptr, const uint8_t* depth_bytes_data, size_t depth_bytes_size)
{
    auto depth_frame{static_cast<DepthDecoder*>(ptr)->decode({depth_bytes_data, depth_bytes_size})};
    return depth_frame.release();
}
//////// END DEPTH DECODER ////////

//////// START DEPTH ENCODER ////////
void* rgbd_depth_encoder_ctor(rgbdDepthCodecType type, int width, int height)
{
    return new DepthEncoder{static_cast<DepthCodecType>(type), width, height};
}

void rgbd_depth_encoder_dtor(void* ptr)
{
    delete static_cast<DepthEncoder*>(ptr);
}

void* rgbd_depth_encoder_encode(void* ptr, const int32_t* depth_values, bool keyframe)
{
    return new NativeByteArray{static_cast<DepthEncoder*>(ptr)->encode(depth_values, keyframe)};
}
//////// END DEPTH DECODER ////////

//////// START DIRECTION TABLE ////////
void rgbd_direction_table_dtor(void* ptr)
{
    delete static_cast<DirectionTable*>(ptr);
}

int rgbd_direction_table_get_width(void* ptr)
{
    return static_cast<DirectionTable*>(ptr)->width();
}

int rgbd_direction_table_get_height(void* ptr)
{
    return static_cast<DirectionTable*>(ptr)->height();
}

size_t rgbd_direction_table_get_direction_count(void* ptr)
{
    return static_cast<DirectionTable*>(ptr)->directions().size();
}

float rgbd_direction_table_get_direction_x(void* ptr, size_t index)
{
    return static_cast<DirectionTable*>(ptr)->directions()[index].x;
}

float rgbd_direction_table_get_direction_y(void* ptr, size_t index)
{
    return static_cast<DirectionTable*>(ptr)->directions()[index].y;
}

float rgbd_direction_table_get_direction_z(void* ptr, size_t index)
{
    return static_cast<DirectionTable*>(ptr)->directions()[index].z;
}
//////// END DIRECTION TABLE ////////

//////// START FRAME MAPPER ////////
void* rgbd_frame_mapper_ctor(void* src_calibration, void* dst_calibration)
{
    return new FrameMapper{*static_cast<const CameraCalibration*>(src_calibration),
                           *static_cast<const CameraCalibration*>(dst_calibration)};
}

void rgbd_frame_mapper_dtor(void* ptr)
{
    delete static_cast<FrameMapper*>(ptr);
}

void* rgbd_frame_mapper_map_color_frame(void* ptr, void* color_frame)
{
    auto frame_mapper{static_cast<FrameMapper*>(ptr)};
    auto mapped_color_frame{frame_mapper->mapColorFrame(*static_cast<YuvFrame*>(color_frame))};
    return mapped_color_frame.release();
}

void* rgbd_frame_mapper_map_depth_frame(void* ptr, void* depth_frame)
{
    auto frame_mapper{static_cast<FrameMapper*>(ptr)};
    auto mapped_depth_frame{frame_mapper->mapDepthFrame(*static_cast<Int32Frame*>(depth_frame))};
    return mapped_depth_frame.release();
}
//////// END FRAME MAPPER ////////

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
    return new KinectCameraCalibration{color_width,
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
    return static_cast<KinectCameraCalibration*>(ptr)->resolution_width();
}

int rgbd_kinect_camera_calibration_get_resolution_height(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->resolution_height();
}

float rgbd_kinect_camera_calibration_get_cx(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->cx();
}

float rgbd_kinect_camera_calibration_get_cy(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->cy();
}

float rgbd_kinect_camera_calibration_get_fx(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->fx();
}

float rgbd_kinect_camera_calibration_get_fy(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->fy();
}

float rgbd_kinect_camera_calibration_get_k1(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->k1();
}

float rgbd_kinect_camera_calibration_get_k2(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->k2();
}

float rgbd_kinect_camera_calibration_get_k3(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->k3();
}

float rgbd_kinect_camera_calibration_get_k4(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->k4();
}

float rgbd_kinect_camera_calibration_get_k5(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->k5();
}

float rgbd_kinect_camera_calibration_get_k6(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->k6();
}

float rgbd_kinect_camera_calibration_get_codx(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->codx();
}

float rgbd_kinect_camera_calibration_get_cody(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->cody();
}

float rgbd_kinect_camera_calibration_get_p1(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->p1();
}

float rgbd_kinect_camera_calibration_get_p2(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->p2();
}

float rgbd_kinect_camera_calibration_get_max_radius_for_projection(void* ptr)
{
    return static_cast<KinectCameraCalibration*>(ptr)->max_radius_for_projection();
}
//////// END KINECT CAMERA CALIBRATION ////////

//////// START INT32 FRAME ////////
void* rgbd_int32_frame_ctor(int width, int height, const int32_t* values)
{
    return new Int32Frame{width, height, values};
}

void rgbd_int32_frame_dtor(void* ptr)
{
    delete static_cast<Int32Frame*>(ptr);
}

int rgbd_int32_frame_get_width(void* ptr)
{
    return static_cast<Int32Frame*>(ptr)->width();
}

int rgbd_int32_frame_get_height(void* ptr)
{
    return static_cast<Int32Frame*>(ptr)->height();
}

void* rgbd_int32_frame_get_values(void* ptr)
{
    return new NativeInt32Array{static_cast<Int32Frame*>(ptr)->values()};
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
    return new IosCameraCalibration{
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
    return static_cast<const IosCameraCalibration*>(ptr)->fx();
}

float rgbd_ios_camera_calibration_get_fy(void* ptr)
{
    return static_cast<const IosCameraCalibration*>(ptr)->fy();
}

float rgbd_ios_camera_calibration_get_ox(void* ptr)
{
    return static_cast<const IosCameraCalibration*>(ptr)->ox();
}

float rgbd_ios_camera_calibration_get_oy(void* ptr)
{
    return static_cast<const IosCameraCalibration*>(ptr)->oy();
}

float rgbd_ios_camera_calibration_get_reference_dimension_width(void* ptr)
{
    return static_cast<const IosCameraCalibration*>(ptr)->reference_dimension_width();
}

float rgbd_ios_camera_calibration_get_reference_dimension_height(void* ptr)
{
    return static_cast<const IosCameraCalibration*>(ptr)->reference_dimension_height();
}

float rgbd_ios_camera_calibration_get_lens_distortion_center_x(void* ptr)
{
    return static_cast<const IosCameraCalibration*>(ptr)->lens_distortion_center_x();
}

float rgbd_ios_camera_calibration_get_lens_distortion_center_y(void* ptr)
{
    return static_cast<const IosCameraCalibration*>(ptr)->lens_distortion_center_y();
}

void* rgbd_ios_camera_calibration_get_lens_distortion_lookup_table(void* ptr)
{
    auto floats{static_cast<const IosCameraCalibration*>(ptr)->lens_distortion_lookup_table()};
    return new NativeFloatArray{std::move(floats)};
}

void* rgbd_ios_camera_calibration_get_inverse_lens_distortion_lookup_table(void* ptr)
{
    auto floats{
        static_cast<const IosCameraCalibration*>(ptr)->inverse_lens_distortion_lookup_table()};
    return new NativeFloatArray{std::move(floats)};
}
//////// END IOS CAMERA CALIBRATION ////////

//////// START MATH UTILS ////////
void* rgbd_math_utils_apply_rotation_rate_and_gravity_to_rotation(void* rotation_ptr,
                                                                  float delta_time_sec,
                                                                  void* rotation_rate_ptr,
                                                                  void* gravity_ptr)
{
    auto rotation{static_cast<NativeQuaternion*>(rotation_ptr)};
    auto rotation_rate{static_cast<NativeVector3*>(rotation_rate_ptr)};
    auto gravity{static_cast<NativeVector3*>(gravity_ptr)};
    glm::quat result{MathUtils::applyRotationRateAndGravityToRotation(
        *rotation, delta_time_sec, *rotation_rate, *gravity)};
    return new NativeQuaternion{result};
}

void* rgbd_math_utils_convert_euler_angles_to_quaternion(void* euler_angles_ptr)
{
    auto euler_angles{static_cast<NativeVector3*>(euler_angles_ptr)};
    glm::quat quat{MathUtils::convertEulerAnglesToQuaternion(*euler_angles)};
    return new NativeQuaternion{quat};
}

void* rgbd_math_utils_convert_quaternion_to_euler_angles(void* quat_ptr)
{
    auto quat{static_cast<NativeQuaternion*>(quat_ptr)};
    glm::vec3 euler_angles{MathUtils::convertQuaternionToEulerAngles(*quat)};
    return new NativeVector3{euler_angles};
}
//////// END MATH UTILS ////////

//////// START RECORD ////////
void rgbd_record_dtor(void* ptr)
{
    delete static_cast<Record*>(ptr);
}

void* rgbd_record_get_info(void* ptr)
{
    return &(static_cast<Record*>(ptr)->info());
}

void* rgbd_record_get_tracks(void* ptr)
{
    return &(static_cast<Record*>(ptr)->tracks());
}

void* rgbd_record_get_attachments(void* ptr)
{
    return &(static_cast<Record*>(ptr)->attachments());
}

size_t rgbd_record_get_video_frame_count(void* ptr)
{
    return static_cast<Record*>(ptr)->video_frames().size();
}

void* rgbd_record_get_video_frame(void* ptr, size_t index)
{
    auto file{static_cast<Record*>(ptr)};
    auto& video_frame{file->video_frames()[index]};
    return &video_frame;
}

size_t rgbd_record_get_audio_frame_count(void* ptr)
{
    return static_cast<Record*>(ptr)->audio_frames().size();
}

void* rgbd_record_get_audio_frame(void* ptr, size_t index)
{
    auto file{static_cast<Record*>(ptr)};
    auto& audio_frame{file->audio_frames()[index]};
    return &audio_frame;
}

size_t rgbd_record_get_imu_frame_count(void* ptr)
{
    return static_cast<Record*>(ptr)->imu_frames().size();
}

void* rgbd_record_get_imu_frame(void* ptr, size_t index)
{
    auto file{static_cast<Record*>(ptr)};
    auto& imu_frame{file->imu_frames()[index]};
    return &imu_frame;
}

size_t rgbd_record_get_pose_frame_count(void* ptr)
{
    return static_cast<Record*>(ptr)->pose_frames().size();
}

void* rgbd_record_get_pose_frame(void* ptr, size_t index)
{
    auto file{static_cast<Record*>(ptr)};
    auto& pose_frame{file->pose_frames()[index]};
    return &pose_frame;
}

bool rgbd_record_has_direction_table(void* ptr)
{
    return static_cast<Record*>(ptr)->direction_table().has_value();
}

void* rgbd_record_get_direction_table(void* ptr)
{
    return &(static_cast<Record*>(ptr)->direction_table().value());
}
//////// END RECORD ////////

//////// START RECORD ATTACHMENTS ////////
void rgbd_record_attachments_dtor(void* ptr)
{
    delete static_cast<RecordAttachments*>(ptr);
}

void* rgbd_record_attachments_get_camera_calibration(void* ptr)
{
    auto file_attachments{static_cast<RecordAttachments*>(ptr)};
    return file_attachments->camera_calibration.get();
}

void* rgbd_record_attachments_get_cover_png_bytes(void* ptr)
{
    auto file_attachments{static_cast<RecordAttachments*>(ptr)};
    if (!file_attachments->cover_png_bytes)
        return nullptr;
    return new NativeByteArray{*file_attachments->cover_png_bytes};
}
//////// END RECORD ATTACHMENTS ////////

//////// START RECORD AUDIO FRAME ////////
void* rgbd_record_audio_frame_ctor(int64_t time_point_us, const uint8_t* bytes_data, size_t byte_size)
{
    Bytes bytes;
    bytes.insert(bytes.end(), &bytes_data[0], &bytes_data[byte_size]);
    return new RecordAudioFrame{time_point_us, bytes};
}

void* rgbd_record_audio_frame_ctor_wasm(int time_point_us,
                                      const uint8_t* bytes_data,
                                      size_t byte_size)
{
    return rgbd_record_audio_frame_ctor(time_point_us, bytes_data, byte_size);
}

void rgbd_record_audio_frame_dtor(void* ptr)
{
    delete static_cast<RecordAudioFrame*>(ptr);
}

int64_t rgbd_record_audio_frame_get_time_point_us(void* ptr)
{
    return static_cast<RecordAudioFrame*>(ptr)->time_point_us();
}

void* rgbd_record_audio_frame_get_bytes(void* ptr)
{
    return new NativeByteArray{static_cast<RecordAudioFrame*>(ptr)->bytes()};
}
//////// END RECORD AUDIO FRAME ////////

//////// START RECORD AUDIO TRACK ////////
void rgbd_record_audio_track_dtor(void* ptr)
{
    delete static_cast<RecordAudioTrack*>(ptr);
}

int rgbd_record_audio_track_get_track_number(void* ptr)
{
    return static_cast<RecordAudioTrack*>(ptr)->track_number;
}

double rgbd_record_audio_track_get_sampling_frequency(void* ptr)
{
    return static_cast<RecordAudioTrack*>(ptr)->sampling_frequency;
}
//////// END RECORD AUDIO TRACK ////////

//////// START RECORD BYTES BUILDER ////////
void* rgbd_record_bytes_builder_ctor()
{
    return new RecordBytesBuilder;
}

void rgbd_record_bytes_builder_dtor(void* ptr)
{
    delete static_cast<RecordBytesBuilder*>(ptr);
}

void rgbd_record_bytes_builder_set_sample_rate(void* ptr, int sample_rate)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    file_bytes_builder->setSampleRate(sample_rate);
}

void rgbd_record_bytes_builder_set_depth_codec_type(void* ptr, rgbdDepthCodecType depth_codec_type)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    file_bytes_builder->setDepthCodecType(static_cast<DepthCodecType>(depth_codec_type));
}

void rgbd_record_bytes_builder_set_depth_unit(void* ptr, float depth_unit)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    file_bytes_builder->setDepthUnit(depth_unit);
}

void rgbd_record_bytes_builder_set_calibration(void* ptr, void* calibration_ptr)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    auto calibration{static_cast<CameraCalibration*>(calibration_ptr)};
    file_bytes_builder->setCalibration(*calibration);
}

void rgbd_record_bytes_builder_set_cover_png_bytes(void* ptr,
                                                 const uint8_t* cover_png_bytes_data,
                                                 size_t cover_png_byte_size)
{
    Bytes cover_png_bytes;
    cover_png_bytes.insert(cover_png_bytes.end(),
                           &cover_png_bytes_data[0],
                           &cover_png_bytes_data[cover_png_byte_size]);

    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    file_bytes_builder->setCoverPNGBytes(cover_png_bytes);
}

void rgbd_record_bytes_builder_add_video_frame(void* ptr, void* video_frame_ptr)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    auto video_frame{static_cast<RecordVideoFrame*>(video_frame_ptr)};
    file_bytes_builder->addVideoFrame(*video_frame);
}

void rgbd_record_bytes_builder_add_audio_frame(void* ptr, void* audio_frame_ptr)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    auto audio_frame{static_cast<RecordAudioFrame*>(audio_frame_ptr)};
    file_bytes_builder->addAudioFrame(*audio_frame);
}

void rgbd_record_bytes_builder_add_imu_frame(void* ptr, void* imu_frame_ptr)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    auto imu_frame{static_cast<RecordIMUFrame*>(imu_frame_ptr)};
    file_bytes_builder->addIMUFrame(*imu_frame);
}

void rgbd_record_bytes_builder_add_pose_frame(void* ptr, void* pose_frame_ptr)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    auto pose_frame{static_cast<RecordPoseFrame*>(pose_frame_ptr)};
    file_bytes_builder->addPoseFrame(*pose_frame);
}

void* rgbd_record_bytes_builder_build(void* ptr)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    return new NativeByteArray{file_bytes_builder->build()};
}

void rgbd_record_bytes_builder_build_to_path(void* ptr, const char* path)
{
    auto file_bytes_builder{static_cast<RecordBytesBuilder*>(ptr)};
    file_bytes_builder->buildToPath(path);
}
//////// END RECORD BYTES BUILDER ////////

//////// START RECORD COLOR VIDEO TRACK ////////
rgbdColorCodecType rgbd_record_color_video_track_get_codec(void* ptr)
{
    auto file_video_track{static_cast<RecordColorVideoTrack*>(ptr)};
    return static_cast<rgbdColorCodecType>(file_video_track->codec);
}
//////// END RECORD COLOR VIDEO TRACK ////////

//////// START RECORD DEPTH VIDEO TRACK ////////
rgbdDepthCodecType rgbd_record_depth_video_track_get_codec(void* ptr)
{
    auto file_video_track{static_cast<RecordDepthVideoTrack*>(ptr)};
    return static_cast<rgbdDepthCodecType>(file_video_track->codec);
}

float rgbd_record_depth_video_track_get_depth_unit(void* ptr)
{
    return static_cast<RecordDepthVideoTrack*>(ptr)->depth_unit;
}
//////// END RECORD DEPTH VIDEO TRACK ////////

//////// START RECORD FRAME ////////
void rgbd_record_frame_dtor(void* ptr)
{
    delete static_cast<RecordFrame*>(ptr);
}

rgbdRecordFrameType rgbd_record_frame_get_type(void* ptr)
{
    auto type{static_cast<RecordFrame*>(ptr)->getType()};
    return static_cast<rgbdRecordFrameType>(type);
}
//////// END RECORD FRAME ////////

//////// START RECORD IMU FRAME ////////
void* rgbd_record_imu_frame_ctor(int64_t time_point_us,
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
    return new RecordIMUFrame{time_point_us,
                              glm::vec3{acceleration_x, acceleration_y, acceleration_z},
                              glm::vec3{rotation_rate_x, rotation_rate_y, rotation_rate_z},
                              glm::vec3{magnetic_field_x, magnetic_field_y, magnetic_field_z},
                              glm::vec3{gravity_x, gravity_y, gravity_z}};
}

void* rgbd_record_imu_frame_ctor_wasm(int time_point_us,
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
    return rgbd_record_imu_frame_ctor(time_point_us,
                                    acceleration_x,
                                    acceleration_y,
                                    acceleration_z,
                                    rotation_rate_x,
                                    rotation_rate_y,
                                    rotation_rate_z,
                                    magnetic_field_x,
                                    magnetic_field_y,
                                    magnetic_field_z,
                                    gravity_x,
                                    gravity_y,
                                    gravity_z);
}

void rgbd_record_imu_frame_dtor(void* ptr)
{
    delete static_cast<RecordIMUFrame*>(ptr);
}

int64_t rgbd_record_imu_frame_get_time_point_us(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->time_point_us();
}

float rgbd_record_imu_frame_get_acceleration_x(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->acceleration().x;
}

float rgbd_record_imu_frame_get_acceleration_y(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->acceleration().y;
}

float rgbd_record_imu_frame_get_acceleration_z(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->acceleration().z;
}

float rgbd_record_imu_frame_get_rotation_rate_x(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->rotation_rate().x;
}

float rgbd_record_imu_frame_get_rotation_rate_y(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->rotation_rate().y;
}

float rgbd_record_imu_frame_get_rotation_rate_z(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->rotation_rate().z;
}

float rgbd_record_imu_frame_get_magnetic_field_x(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->magnetic_field().x;
}

float rgbd_record_imu_frame_get_magnetic_field_y(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->magnetic_field().y;
}

float rgbd_record_imu_frame_get_magnetic_field_z(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->magnetic_field().z;
}
float rgbd_record_imu_frame_get_gravity_x(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->gravity().x;
}

float rgbd_record_imu_frame_get_gravity_y(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->gravity().y;
}

float rgbd_record_imu_frame_get_gravity_z(void* ptr)
{
    return static_cast<RecordIMUFrame*>(ptr)->gravity().z;
}
//////// END RECORD IMU FRAME ////////

//////// START RECORD INFO ////////
void rgbd_record_info_dtor(void* ptr)
{
    delete static_cast<RecordInfo*>(ptr);
}

uint64_t rgbd_record_info_get_timecode_scale_ns(void* ptr)
{
    auto file_info{static_cast<RecordInfo*>(ptr)};
    return file_info->timecode_scale_ns;
}

double rgbd_record_info_get_duration_us(void* ptr)
{
    auto file_info{static_cast<RecordInfo*>(ptr)};
    return file_info->duration_us;
}

void* rgbd_record_info_get_writing_app(void* ptr)
{
    auto file_info{static_cast<RecordInfo*>(ptr)};
    return new NativeString{file_info->writing_app};
}
//////// END RECORD INFO ////////

//////// START RECORD PARSER ////////
int rgbd_record_parser_ctor_from_data(void** parser_ptr_ref, const void* data_ptr, size_t data_size)
{
    try {
        *parser_ptr_ref = new RecordParser{data_ptr, data_size};
        return 0;
    } catch (std::runtime_error e) {
        spdlog::error("error from rgbd_record_parser_ctor_from_data: {}", e.what());
        return -1;
    }
}

void* rgbd_record_parser_ctor_from_path(const char* file_path)
{
    try {
        return new RecordParser{file_path};
    } catch (std::runtime_error e) {
        spdlog::error("error from rgbd_record_parser_ctor: {}", e.what());
        return nullptr;
    }
}

void rgbd_record_parser_dtor(void* ptr)
{
    delete static_cast<RecordParser*>(ptr);
}

void* rgbd_record_parser_parse(void* ptr, bool with_frames, bool with_directions)
{
    auto file_parser{static_cast<RecordParser*>(ptr)};
    return file_parser->parse(with_frames, with_directions).release();
}
//////// END RECORD PARSER ////////

//////// START RECORD POSE FRAME ////////
void* rgbd_record_pose_frame_ctor(int64_t time_point_us,
                                  float translation_x,
                                  float translation_y,
                                  float translation_z,
                                  float rotation_w,
                                  float rotation_x,
                                  float rotation_y,
                                  float rotation_z)
{
    return new RecordPoseFrame{time_point_us,
                               glm::vec3{translation_x, translation_y, translation_z},
                               glm::quat{rotation_w, rotation_x, rotation_y, rotation_z}};
}

void* rgbd_record_pose_frame_ctor_wasm(int time_point_us,
                                       float translation_x,
                                       float translation_y,
                                       float translation_z,
                                       float rotation_w,
                                       float rotation_x,
                                       float rotation_y,
                                       float rotation_z)
{
    return rgbd_record_pose_frame_ctor(time_point_us,
                                       translation_x,
                                       translation_y,
                                       translation_z,
                                       rotation_w,
                                       rotation_x,
                                       rotation_y,
                                       rotation_z);
}

void rgbd_record_pose_frame_dtor(void* ptr)
{
    delete static_cast<RecordPoseFrame*>(ptr);
}

int64_t rgbd_record_pose_frame_get_time_point_us(void* ptr)
{
    return static_cast<RecordPoseFrame*>(ptr)->time_point_us();
}

float rgbd_record_pose_frame_get_translation_x(void* ptr)
{
    return static_cast<RecordPoseFrame*>(ptr)->translation().x;
}

float rgbd_record_pose_frame_get_translation_y(void* ptr)
{
    return static_cast<RecordPoseFrame*>(ptr)->translation().y;
}

float rgbd_record_pose_frame_get_translation_z(void* ptr)
{
    return static_cast<RecordPoseFrame*>(ptr)->translation().z;
}

float rgbd_record_pose_frame_get_rotation_w(void* ptr)
{
    return static_cast<RecordPoseFrame*>(ptr)->rotation().w;
}

float rgbd_record_pose_frame_get_rotation_x(void* ptr)
{
    return static_cast<RecordPoseFrame*>(ptr)->rotation().x;
}

float rgbd_record_pose_frame_get_rotation_y(void* ptr)
{
    return static_cast<RecordPoseFrame*>(ptr)->rotation().y;
}

float rgbd_record_pose_frame_get_rotation_z(void* ptr)
{
    return static_cast<RecordPoseFrame*>(ptr)->rotation().z;
}
//////// END RECORD POSE FRAME ////////

//////// START RECORD TRACKS ////////
void rgbd_record_tracks_dtor(void* ptr)
{
    delete static_cast<RecordTracks*>(ptr);
}

void* rgbd_record_tracks_get_color_track(void* ptr)
{
    auto file_tracks{static_cast<RecordTracks*>(ptr)};
    return &file_tracks->color_track;
}

void* rgbd_record_tracks_get_depth_track(void* ptr)
{
    auto file_tracks{static_cast<RecordTracks*>(ptr)};
    return &file_tracks->depth_track;
}

void* rgbd_record_tracks_get_audio_track(void* ptr)
{
    auto file_tracks{static_cast<RecordTracks*>(ptr)};
    return &(file_tracks->audio_track);
}
//////// START RECORD TRACKS ////////

//////// START RECORD VIDEO FRAME ////////
void* rgbd_record_video_frame_ctor(int64_t time_point_us,
                                 bool keyframe,
                                 const uint8_t* color_bytes_data,
                                 size_t color_byte_size,
                                 const uint8_t* depth_bytes_data,
                                 size_t depth_byte_size)
{
    Bytes color_bytes;
    color_bytes.insert(color_bytes.end(), &color_bytes_data[0], &color_bytes_data[color_byte_size]);

    Bytes depth_bytes;
    depth_bytes.insert(depth_bytes.end(), &depth_bytes_data[0], &depth_bytes_data[depth_byte_size]);

    return new RecordVideoFrame{time_point_us, keyframe, color_bytes, depth_bytes};
}

void* rgbd_record_video_frame_ctor_wasm(int time_point_us,
                                      bool keyframe,
                                      const uint8_t* color_bytes_data,
                                      size_t color_byte_size,
                                      const uint8_t* depth_bytes_data,
                                      size_t depth_byte_size)
{
    return rgbd_record_video_frame_ctor(time_point_us,
                                      keyframe,
                                      color_bytes_data,
                                      color_byte_size,
                                      depth_bytes_data,
                                      depth_byte_size);
}

void rgbd_record_video_frame_dtor(void* ptr)
{
    delete static_cast<RecordVideoFrame*>(ptr);
}

int64_t rgbd_record_video_frame_get_time_point_us(void* ptr)
{
    return static_cast<RecordVideoFrame*>(ptr)->time_point_us();
}

bool rgbd_record_video_frame_get_keyframe(void* ptr)
{
    return static_cast<RecordVideoFrame*>(ptr)->keyframe();
}

void* rgbd_record_video_frame_get_color_bytes(void* ptr)
{
    return new NativeByteArray{static_cast<RecordVideoFrame*>(ptr)->color_bytes()};
}

void* rgbd_record_video_frame_get_depth_bytes(void* ptr)
{
    return new NativeByteArray{static_cast<RecordVideoFrame*>(ptr)->depth_bytes()};
}
//////// END RECORD VIDEO FRAME ////////

//////// START RECORD VIDEO TRACK ////////
void rgbd_record_video_track_dtor(void* ptr)
{
    delete static_cast<RecordVideoTrack*>(ptr);
}

int rgbd_record_video_track_get_track_number(void* ptr)
{
    auto file_video_track{static_cast<RecordVideoTrack*>(ptr)};
    return file_video_track->track_number;
}

int rgbd_record_video_track_get_width(void* ptr)
{
    auto file_video_track{static_cast<RecordVideoTrack*>(ptr)};
    return file_video_track->width;
}

int rgbd_record_video_track_get_height(void* ptr)
{
    auto file_video_track{static_cast<RecordVideoTrack*>(ptr)};
    return file_video_track->height;
}
//////// END RECORD VIDEO TRACK ////////

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
    return new UndistortedCameraCalibration{
        color_width, color_height, depth_width, depth_height, fx, fy, cx, cy};
}

float rgbd_undistorted_camera_calibration_get_fx(void* ptr)
{
    return static_cast<const UndistortedCameraCalibration*>(ptr)->fx();
}

float rgbd_undistorted_camera_calibration_get_fy(void* ptr)
{
    return static_cast<const UndistortedCameraCalibration*>(ptr)->fy();
}

float rgbd_undistorted_camera_calibration_get_cx(void* ptr)
{
    return static_cast<const UndistortedCameraCalibration*>(ptr)->cx();
}

float rgbd_undistorted_camera_calibration_get_cy(void* ptr)
{
    return static_cast<const UndistortedCameraCalibration*>(ptr)->cy();
}
//////// END UNDISTORTED CAMERA CALIBRATION ////////

//////// START YUV FRAME ////////
void* rgbd_yuv_frame_ctor(int width,
                          int height,
                          const uint8_t* y_channel,
                          const uint8_t* u_channel,
                          const uint8_t* v_channel)
{
    return new YuvFrame{width, height, y_channel, u_channel, v_channel};
}

void rgbd_yuv_frame_dtor(void* ptr)
{
    delete static_cast<YuvFrame*>(ptr);
}

void* rgbd_yuv_frame_get_mkv_cover_sized(void* ptr)
{
    return static_cast<YuvFrame*>(ptr)->getMkvCoverSized().release();
}

void* rgbd_yuv_frame_get_png_bytes(void* ptr)
{
    auto yuv_frame{static_cast<YuvFrame*>(ptr)};
    return new NativeByteArray{yuv_frame->getPNGBytes()};
}

int rgbd_yuv_frame_get_width(void* ptr)
{
    return static_cast<YuvFrame*>(ptr)->width();
}

int rgbd_yuv_frame_get_height(void* ptr)
{
    return static_cast<YuvFrame*>(ptr)->height();
}

void* rgbd_yuv_frame_get_y_channel(void* ptr)
{
    return new NativeUInt8Array{static_cast<YuvFrame*>(ptr)->y_channel()};
}

void* rgbd_yuv_frame_get_u_channel(void* ptr)
{
    return new NativeUInt8Array{static_cast<YuvFrame*>(ptr)->u_channel()};
}

void* rgbd_yuv_frame_get_v_channel(void* ptr)
{
    return new NativeUInt8Array{static_cast<YuvFrame*>(ptr)->v_channel()};
}
//////// END YUV FRAME ////////
