#include "rgbd_capi.h"

#include "capi_utils.hpp"
#include "ffmpeg_audio_decoder.hpp"
#include "ffmpeg_video_decoder.hpp"
#include "file.hpp"
#include "file_parser.hpp"
#include "integer_frame.hpp"
#include "ios_camera_calibration.hpp"
#include "kinect_camera_calibration.hpp"
#include "file_writer.hpp"
#include "tdc1_decoder.hpp"

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

//////// START CAPI UTILITY CLASSES ////////
void rgbd_cbyte_array_dtor(void* ptr)
{
    delete static_cast<rgbd::CByteArray*>(ptr);
}

uint8_t* rgbd_cbyte_array_data(void* ptr)
{
    return static_cast<rgbd::CByteArray*>(ptr)->data();
}

size_t rgbd_cbyte_array_size(void* ptr)
{
    return static_cast<rgbd::CByteArray*>(ptr)->size();
}

void rgbd_cfloat_array_dtor(void* ptr)
{
    delete static_cast<rgbd::CFloatArray*>(ptr);
}

float* rgbd_cfloat_array_data(void* ptr)
{
    return static_cast<rgbd::CFloatArray*>(ptr)->data();
}

size_t rgbd_cfloat_array_size(void* ptr)
{
    return static_cast<rgbd::CFloatArray*>(ptr)->size();
}

void rgbd_cint16_array_dtor(void* ptr)
{
    delete static_cast<rgbd::CInt16Array*>(ptr);
}

int16_t* rgbd_cint16_array_data(void* ptr)
{
    return static_cast<rgbd::CInt16Array*>(ptr)->data();
}

size_t rgbd_cint16_array_size(void* ptr)
{
    return static_cast<rgbd::CInt16Array*>(ptr)->size();
}

void rgbd_cuint8_array_dtor(void* ptr)
{
    delete static_cast<rgbd::CUInt8Array*>(ptr);
}

uint8_t* rgbd_cuint8_array_data(void* ptr)
{
    return static_cast<rgbd::CUInt8Array*>(ptr)->data();
}

size_t rgbd_cuint8_array_size(void* ptr)
{
    return static_cast<rgbd::CUInt8Array*>(ptr)->size();
}

void rgbd_cstring_dtor(void* ptr)
{
    delete static_cast<rgbd::CString*>(ptr);
}
const char* rgbd_cstring_c_str(void* ptr)
{
    return static_cast<rgbd::CString*>(ptr)->c_str();
}
//////// END CAPI UTILITY CLASSES ////////

//////// START CAMERA CALIBRATION ////////
void rgbd_camera_calibration_dtor(void* ptr)
{
    delete static_cast<const rgbd::CameraCalibration*>(ptr);
}

rgbdCameraDeviceType rgbd_camera_calibration_get_camera_device_type(void* ptr)
{
    auto camera_device_type{
        static_cast<const rgbd::CameraCalibration*>(ptr)->getCameraDeviceType()};
    return static_cast<rgbdCameraDeviceType>(camera_device_type);
}
//////// END CAMERA CALIBRATION ////////

//////// START FFMPEG AUDIO DECODER ////////
void* rgbd_ffmpeg_audio_decoder_ctor()
{
    return new rgbd::FFmpegAudioDecoder;
}

void rgbd_ffmpeg_audio_decoder_dtor(void* ptr)
{
    delete static_cast<rgbd::FFmpegAudioDecoder*>(ptr);
}

void* rgbd_ffmpeg_audio_decoder_decode(void* ptr,
                                       const uint8_t* opus_frame_data,
                                       size_t opus_frame_size)
{
    auto depth_frame{static_cast<rgbd::FFmpegAudioDecoder*>(ptr)->decode(
        {reinterpret_cast<const std::byte*>(opus_frame_data), opus_frame_size})};
    return new rgbd::CFloatArray{std::move(depth_frame)};
}
//////// END FFMPEG AUDIO DECODER ////////

//////// START FFMPEG VIDEO DECODER ////////
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
//////// END FFMPEG VIDEO DECODER ////////

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

void* rgbd_file_get_camera_calibration(void* ptr)
{
    return static_cast<rgbd::File*>(ptr)->camera_calibration();
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
    return new rgbd::CByteArray{file_attachments->cover_png_bytes};
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
    return new rgbd::CByteArray{static_cast<rgbd::FileAudioFrame*>(ptr)->bytes()};
}
//////// END FILE AUDIO FRAME ////////

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
    return new rgbd::CString{file_info->writing_app};
}
//////// END FILE INFO ////////

//////// START FILE PARSER ////////
void* rgbd_file_parser_ctor_from_data(void* ptr, size_t size)
{
    try {
        return new rgbd::FileParser{ptr, size};
    } catch (std::runtime_error e) {
        spdlog::error("error from rgbd_file_parser_ctor: {}", e.what());
        return nullptr;
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

void* rgbd_file_parser_parse_no_frames(void* ptr)
{
    auto file_parser{static_cast<rgbd::FileParser*>(ptr)};
    return file_parser->parseNoFrames().release();
}

void* rgbd_file_parser_parse_all_frames(void* ptr)
{
    auto file_parser{static_cast<rgbd::FileParser*>(ptr)};
    return file_parser->parseAllFrames().release();
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

void* rgbd_file_tracks_get_depth_confidence_track(void* ptr)
{
    auto file_tracks{static_cast<rgbd::FileTracks*>(ptr)};
    if (!file_tracks->depth_confidence_track)
        return nullptr;
    return &(*file_tracks->depth_confidence_track);
}

int rgbd_file_tracks_get_audio_track_number(void* ptr)
{
    auto file_tracks{static_cast<rgbd::FileTracks*>(ptr)};
    return file_tracks->audio_track_number;
}

int rgbd_file_tracks_get_floor_track_number(void* ptr)
{
    auto file_tracks{static_cast<rgbd::FileTracks*>(ptr)};
    return file_tracks->floor_track_number;
}
//////// START FILE TRACKS ////////

//////// START FILE VIDEO FRAME ////////
void rgbd_file_video_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::FileVideoFrame*>(ptr);
}

int64_t rgbd_file_video_frame_get_global_timecode(void* ptr)
{
    return static_cast<rgbd::FileVideoFrame*>(ptr)->global_timecode();
}

void* rgbd_file_video_frame_get_color_bytes(void* ptr)
{
    return new rgbd::CByteArray{static_cast<rgbd::FileVideoFrame*>(ptr)->color_bytes()};
}

void* rgbd_file_video_frame_get_depth_bytes(void* ptr)
{
    return new rgbd::CByteArray{static_cast<rgbd::FileVideoFrame*>(ptr)->depth_bytes()};
}

float rgbd_file_video_frame_get_floor_normal_x(void* ptr)
{
    return static_cast<rgbd::FileVideoFrame*>(ptr)->floor().normal().x;
}

float rgbd_file_video_frame_get_floor_normal_y(void* ptr)
{
    return static_cast<rgbd::FileVideoFrame*>(ptr)->floor().normal().y;
}

float rgbd_file_video_frame_get_floor_normal_z(void* ptr)
{
    return static_cast<rgbd::FileVideoFrame*>(ptr)->floor().normal().z;
}

float rgbd_file_video_frame_get_floor_constant(void* ptr)
{
    return static_cast<rgbd::FileVideoFrame*>(ptr)->floor().constant();
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
    return new rgbd::CString{file_video_track->codec};
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
void* rgbd_file_writer_ctor(const char* file_path,
                            bool has_depth_confidence,
                            void* calibration,
                            int color_bitrate,
                            int framerate,
                            int depth_diff_multiplier,
                            int samplerate)
{
    return new rgbd::FileWriter(file_path,
                                has_depth_confidence,
                                *static_cast<const rgbd::CameraCalibration*>(calibration),
                                color_bitrate,
                                framerate,
                                depth_diff_multiplier,
                                samplerate);
}

void rgbd_file_writer_dtor(void* ptr)
{
    delete static_cast<rgbd::FileWriter*>(ptr);
}

void rgbd_file_writer_write_video_frame(void* ptr,
                                        int64_t time_point_us,
                                        int width,
                                        int height,
                                        const uint8_t* y_channel,
                                        size_t y_channel_size,
                                        const uint8_t* u_channel,
                                        size_t u_channel_size,
                                        const uint8_t* v_channel,
                                        size_t v_channel_size,
                                        const int16_t* depth_values,
                                        size_t depth_values_size,
                                        const uint8_t* depth_confidence_values,
                                        size_t depth_confidence_values_size,
                                        float floor_normal_x,
                                        float floor_normal_y,
                                        float floor_normal_z,
                                        float floor_distance)
{
    glm::vec3 floor_normal{floor_normal_x, floor_normal_y, floor_normal_z};
    if (depth_confidence_values) {
        static_cast<rgbd::FileWriter*>(ptr)->writeVideoFrame(
            time_point_us,
            width,
            height,
            gsl::span<const uint8_t>{y_channel, y_channel_size},
            gsl::span<const uint8_t>{u_channel, u_channel_size},
            gsl::span<const uint8_t>{v_channel, v_channel_size},
            gsl::span<const int16_t>{depth_values, depth_values_size},
            gsl::span<const uint8_t>{depth_confidence_values, depth_confidence_values_size},
            rgbd::Plane{floor_normal, floor_distance});
    } else {
        static_cast<rgbd::FileWriter*>(ptr)->writeVideoFrame(
            time_point_us,
            width,
            height,
            gsl::span<const uint8_t>{y_channel, y_channel_size},
            gsl::span<const uint8_t>{u_channel, u_channel_size},
            gsl::span<const uint8_t>{v_channel, v_channel_size},
            gsl::span<const int16_t>{depth_values, depth_values_size},
            std::nullopt,
            rgbd::Plane{floor_normal, floor_distance});
    }
}

void rgbd_file_writer_write_audio_frame(void* ptr,
                                        int64_t time_point_us,
                                        const float* pcm_samples,
                                        size_t pcm_samples_size)
{
    static_cast<rgbd::FileWriter*>(ptr)->writeAudioFrame(
        time_point_us, gsl::span<const float>{pcm_samples, pcm_samples_size});
}

void rgbd_file_writer_flush(void* ptr)
{
    return static_cast<rgbd::FileWriter*>(ptr)->flush();
}
//////// END FILE WRITER ////////

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

int rgbd_kinect_camera_calibration_get_color_width(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->getColorWidth();
}

int rgbd_kinect_camera_calibration_get_color_height(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->getColorHeight();
}

int rgbd_kinect_camera_calibration_get_depth_width(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->getDepthWidth();
}

int rgbd_kinect_camera_calibration_get_depth_height(void* ptr)
{
    return static_cast<rgbd::KinectCameraCalibration*>(ptr)->getDepthHeight();
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

//////// START INT16 FRAME ////////
void rgbd_int16_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::Int16Frame*>(ptr);
}

int rgbd_int16_frame_get_width(void* ptr)
{
    return static_cast<rgbd::Int16Frame*>(ptr)->width();
}

int rgbd_int16_frame_get_height(void* ptr)
{
    return static_cast<rgbd::Int16Frame*>(ptr)->height();
}

void* rgbd_int16_frame_get_values(void* ptr)
{
    return new rgbd::CInt16Array{static_cast<rgbd::Int16Frame*>(ptr)->values()};
}
//////// END INT16 FRAME ////////

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
                                       size_t lens_distortion_lookup_table_size)
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
        {lens_distortion_lookup_table, lens_distortion_lookup_table_size}};
}

int rgbd_ios_camera_calibration_get_color_width(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->getColorWidth();
}

int rgbd_ios_camera_calibration_get_color_height(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->getColorHeight();
}

int rgbd_ios_camera_calibration_get_depth_width(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->getDepthWidth();
}

int rgbd_ios_camera_calibration_get_depth_height(void* ptr)
{
    return static_cast<const rgbd::IosCameraCalibration*>(ptr)->getDepthHeight();
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
    return new rgbd::CFloatArray{std::move(floats)};
}
//////// END IOS CAMERA CALIBRATION ////////

//////// START TDC1 DECODER ////////
void* rgbd_tdc1_decoder_ctor()
{
    return new rgbd::TDC1Decoder;
}

void rgbd_tdc1_decoder_dtor(void* ptr)
{
    delete static_cast<rgbd::TDC1Decoder*>(ptr);
}

void* rgbd_tdc1_decoder_decode(void* ptr,
                               const uint8_t* encoded_depth_frame_data,
                               size_t encoded_depth_frame_size)
{
    auto depth_frame{static_cast<rgbd::TDC1Decoder*>(ptr)->decode(
        {reinterpret_cast<const std::byte*>(encoded_depth_frame_data), encoded_depth_frame_size})};
    return new rgbd::Int16Frame{std::move(depth_frame)};
}
//////// END TDC1 DECODER ////////

//////// START YUV FRAME ////////
void rgbd_yuv_frame_dtor(void* ptr)
{
    delete static_cast<rgbd::YuvFrame*>(ptr);
}

void* rgbd_yuv_frame_get_y_channel(void* ptr)
{
    return new rgbd::CUInt8Array{static_cast<rgbd::YuvFrame*>(ptr)->y_channel()};
}

void* rgbd_yuv_frame_get_u_channel(void* ptr)
{
    return new rgbd::CUInt8Array{static_cast<rgbd::YuvFrame*>(ptr)->u_channel()};
}

void* rgbd_yuv_frame_get_v_channel(void* ptr)
{
    return new rgbd::CUInt8Array{static_cast<rgbd::YuvFrame*>(ptr)->v_channel()};
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
