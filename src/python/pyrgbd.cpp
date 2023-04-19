#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <rgbd/rgbd.hpp>

namespace py = pybind11;
using namespace rgbd;

glm::quat read_py_quat(const py::object& py_quat)
{
    float w{py_quat.attr("w").cast<float>()};
    float x{py_quat.attr("x").cast<float>()};
    float y{py_quat.attr("y").cast<float>()};
    float z{py_quat.attr("z").cast<float>()};
    return glm::quat{w, x, y, z};
}

glm::vec3 read_py_vec3(const py::object& py_vec3)
{
    float x{py_vec3.attr("x").cast<float>()};
    float y{py_vec3.attr("y").cast<float>()};
    float z{py_vec3.attr("z").cast<float>()};
    return glm::vec3{x, y, z};
}

py::object create_py_quat(py::module_ glm, const glm::quat& quat)
{
    return glm.attr("quat")(quat.w, quat.x, quat.y, quat.z);
}

py::object create_py_vec3(py::module_ glm, const glm::vec3& vec3)
{
    return glm.attr("vec3")(vec3.x, vec3.y, vec3.z);
}

PYBIND11_MODULE(pyrgbd, m)
{
    m.doc() = "Library for reading and writing RGBD videos.";

    // BEGIN audio_decoder.hpp
    py::class_<AudioDecoder>(m, "AudioDecoder")
        .def(py::init())
        .def("decode",
             [](AudioDecoder& decoder, const Bytes& bytes) {
                 return decoder.decode({bytes.data(), bytes.size()});
             })
        .def("decode", &AudioDecoder::decode);
    // END audio_decoder.hpp

    // BEGIN audio_encoder.hpp
    py::class_<AudioEncoderFrame>(m, "AudioEncoderFrame")
        .def(py::init())
        .def_readwrite("packet_bytes_list", &AudioEncoderFrame::packet_bytes_list);

    py::class_<AudioEncoder>(m, "AudioEncoder")
        .def(py::init())
        .def("packet_bytes_list", &AudioEncoder::encode)
        .def("flush", &AudioEncoder::flush)
        .def_property_readonly("codec_context", &AudioEncoder::codec_context)
        .def_property_readonly("next_pts", &AudioEncoder::next_pts);
    // END audio_encoder.hpp

    // BEGIN audio_frame.hpp
    py::class_<AudioFrame>(m, "AudioFrame")
        .def(py::init<int64_t, const vector<float>&>())
        .def_property_readonly("time_point_us", &AudioFrame::time_point_us)
        .def_property_readonly("pcm_samples", &AudioFrame::pcm_samples);
    // END audio_frame.hpp

    // BEGIN camera_calibration.hpp
    py::class_<CameraCalibration, std::shared_ptr<CameraCalibration>>(m, "CameraCalibration")
        .def_property_readonly("type", &CameraCalibration::getType)
        .def_property_readonly("color_width", &CameraCalibration::getColorWidth)
        .def_property_readonly("color_height", &CameraCalibration::getColorHeight)
        .def_property_readonly("depth_width", &CameraCalibration::getDepthWidth)
        .def_property_readonly("depth_height", &CameraCalibration::getDepthHeight)
        .def("get_direction", &CameraCalibration::getDirection)
        .def("get_uv", &CameraCalibration::getUv);
    // END camera_calibration.hpp

    // BEGIN color_decoder.hpp
    py::class_<ColorDecoder>(m, "ColorDecoder")
        .def(py::init<ColorCodecType>())
        .def("decode", [](ColorDecoder& decoder, const Bytes& bytes) {
            auto yuv_frame{decoder.decode({bytes.data(), bytes.size()})};
            return YuvFrame{std::move(*yuv_frame)};
        });
    // END color_decoder.hpp

    // BEGIN color_encoder.hpp
    py::class_<ColorEncoder>(m, "ColorEncoder")
        .def(py::init<ColorCodecType, int, int>())
        .def("encode", &ColorEncoder::encode);
    // END color_encoder.hpp

    // BEGIN constants.hpp
    py::enum_<CameraCalibrationType>(m, "CameraCalibrationType")
        .value("AzureKinect", CameraCalibrationType::AzureKinect)
        .value("IOS", CameraCalibrationType::IOS)
        .value("Undistorted", CameraCalibrationType::Undistorted);

    py::enum_<ColorCodecType>(m, "ColorCodecType").value("VP8", ColorCodecType::VP8);

    py::enum_<DepthCodecType>(m, "DepthCodecType")
        .value("RVL", DepthCodecType::RVL)
        .value("TDC1", DepthCodecType::TDC1);

    m.attr("VIDEO_FRAME_RATE") = VIDEO_FRAME_RATE;
    m.attr("AUDIO_SAMPLE_RATE") = AUDIO_SAMPLE_RATE;
    m.attr("AUDIO_INPUT_SAMPLES_PER_FRAME") = AUDIO_INPUT_SAMPLES_PER_FRAME;
    // END constants.hpp

    // BEGIN depth_decoder.hpp
    py::class_<DepthDecoder>(m, "DepthDecoder")
        .def(py::init<DepthCodecType>())
        .def("decode", [](DepthDecoder& decoder, const Bytes& bytes) {
            auto depth_frame{decoder.decode({bytes.data(), bytes.size()})};
            return Int32Frame{std::move(*depth_frame)};
        });
    // END depth_decoder.hpp

    // BEGIN depth_encoder.hpp
    py::class_<DepthEncoder>(m, "DepthEncoder")
        .def(py::init<DepthCodecType, int, int>())
        .def_property_readonly("codec_type", &DepthEncoder::getCodecType)
        .def("encode",
             [](DepthEncoder& encoder, const py::array_t<int32_t> depth_array, bool keyframe) {
                 py::buffer_info depth_buffer{depth_array.request()};
                 return encoder.encode(static_cast<int32_t*>(depth_buffer.ptr), keyframe);
             });
    // END depth_encoder.hpp

    // BEGIN direction_table.hpp
    py::class_<DirectionTable>(m, "DirectionTable")
        .def_property_readonly("width", &DirectionTable::width)
        .def_property_readonly("height", &DirectionTable::height)
        .def("get_directions", [](const DirectionTable& direction_table) {
            auto directions{direction_table.directions()};
            py::array_t<float> array(directions.size() * 3);
            auto buffer{array.request()};
            float* ptr{static_cast<float*>(buffer.ptr)};
            for (size_t i{0}; i < directions.size(); ++i) {
                ptr[i * 3 + 0] = directions[i].x;
                ptr[i * 3 + 1] = directions[i].y;
                ptr[i * 3 + 2] = directions[i].z;
            }
            array = array.reshape({direction_table.height(), direction_table.width(), 3});
            return array;
        });
    // END depth_encoder.hpp

    // BEGIN record.hpp
    py::class_<RecordOffsets>(m, "RecordOffsets")
        .def(py::init())
        .def_readwrite("segment_info_offset", &RecordOffsets::segment_info_offset)
        .def_readwrite("tracks_offset", &RecordOffsets::tracks_offset)
        .def_readwrite("attachments_offset", &RecordOffsets::attachments_offset)
        .def_readwrite("first_cluster_offset", &RecordOffsets::first_cluster_offset);

    py::class_<RecordInfo>(m, "RecordInfo")
        .def(py::init())
        .def_readwrite("timecode_scale_ns", &RecordInfo::timecode_scale_ns)
        .def_readwrite("duration_us", &RecordInfo::duration_us)
        .def_readwrite("writing_app", &RecordInfo::writing_app);

    py::class_<RecordVideoTrack>(m, "RecordVideoTrack")
        .def(py::init())
        .def_readwrite("track_number", &RecordVideoTrack::track_number)
        .def_readwrite("default_duration_ns", &RecordVideoTrack::default_duration_ns)
        .def_readwrite("width", &RecordVideoTrack::width)
        .def_readwrite("height", &RecordVideoTrack::height);

    py::class_<RecordColorVideoTrack, RecordVideoTrack>(m, "RecordColorVideoTrack")
        .def(py::init())
        .def_readwrite("codec", &RecordColorVideoTrack::codec);

    py::class_<RecordDepthVideoTrack, RecordVideoTrack>(m, "RecordDepthVideoTrack")
        .def(py::init())
        .def_readwrite("codec", &RecordDepthVideoTrack::codec)
        .def_readwrite("depth_unit", &RecordDepthVideoTrack::depth_unit);

    py::class_<RecordAudioTrack>(m, "RecordAudioTrack")
        .def(py::init())
        .def_readwrite("track_number", &RecordAudioTrack::track_number)
        .def_readwrite("sampling_frequency", &RecordAudioTrack::sampling_frequency);

    py::class_<RecordTracks>(m, "RecordTracks")
        .def(py::init())
        .def_readwrite("color_track", &RecordTracks::color_track)
        .def_readwrite("depth_track", &RecordTracks::depth_track)
        .def_readwrite("audio_track", &RecordTracks::audio_track)
        .def_readwrite("acceleration_track_number", &RecordTracks::acceleration_track_number)
        .def_readwrite("rotation_rate_track_number", &RecordTracks::rotation_rate_track_number)
        .def_readwrite("magnetic_field_track_number", &RecordTracks::magnetic_field_track_number)
        .def_readwrite("gravity_track_number", &RecordTracks::gravity_track_number)
        .def_readwrite("translation_track_number", &RecordTracks::translation_track_number)
        .def_readwrite("rotation_track_number", &RecordTracks::rotation_track_number)
        .def_readwrite("calibration_track_number", &RecordTracks::calibration_track_number);

    py::class_<RecordAttachments>(m, "RecordAttachments")
        .def(py::init())
        .def_readwrite("camera_calibration", &RecordAttachments::camera_calibration)
        .def_readwrite("cover_png_bytes", &RecordAttachments::cover_png_bytes);

    py::enum_<RecordFrameType>(m, "RecordFrameType")
        .value("Video", RecordFrameType::Video)
        .value("Audio", RecordFrameType::Audio)
        .value("IMU", RecordFrameType::IMU)
        .value("Pose", RecordFrameType::Pose);

    py::class_<RecordFrame>(m, "RecordFrame").def("getType", &RecordFrame::getType);

    py::class_<RecordVideoFrame, RecordFrame>(m, "RecordVideoFrame")
        .def(py::init<int64_t, bool, const Bytes&, const Bytes&>())
        .def_property_readonly("time_point_us", &RecordVideoFrame::time_point_us)
        .def_property_readonly("keyframe", &RecordVideoFrame::keyframe)
        .def("get_color_bytes", &RecordVideoFrame::color_bytes, py::return_value_policy::copy)
        .def("get_depth_bytes", &RecordVideoFrame::depth_bytes, py::return_value_policy::copy);

    py::class_<RecordAudioFrame, RecordFrame>(m, "RecordAudioFrame")
        .def(py::init<int64_t, const Bytes&>())
        .def_property_readonly("time_point_us", &RecordAudioFrame::time_point_us)
        .def("get_bytes", &RecordAudioFrame::bytes, py::return_value_policy::copy);

    py::class_<RecordIMUFrame, RecordFrame>(m, "RecordIMUFrame")
        .def(py::init([](int64_t time_point_us,
                         const py::object& py_acceleration,
                         const py::object& py_rotation_rate,
                         const py::object& py_magnetic_field,
                         const py::object& py_gravity) {
            glm::vec3 acceleration{read_py_vec3(py_acceleration)};
            glm::vec3 rotation_rate{read_py_vec3(py_rotation_rate)};
            glm::vec3 magnetic_field{read_py_vec3(py_magnetic_field)};
            glm::vec3 gravity{read_py_vec3(py_gravity)};
            return RecordIMUFrame{time_point_us, acceleration, rotation_rate, magnetic_field, gravity};
        }))
        .def(py::init<int64_t,
                      const glm::vec3&,
                      const glm::vec3&,
                      const glm::vec3&,
                      const glm::vec3&>())
        .def_property_readonly("time_point_us", &RecordIMUFrame::time_point_us)
        .def_property_readonly("acceleration",
                               [](const RecordIMUFrame& frame) {
                                   py::module_ glm{py::module_::import("glm")};
                                   return create_py_vec3(glm, frame.acceleration());
                               })
        .def_property_readonly("rotation_rate",
                               [](const RecordIMUFrame& frame) {
                                   py::module_ glm{py::module_::import("glm")};
                                   return create_py_vec3(glm, frame.rotation_rate());
                               })
        .def_property_readonly("magnetic_field",
                               [](const RecordIMUFrame& frame) {
                                   py::module_ glm{py::module_::import("glm")};
                                   return create_py_vec3(glm, frame.magnetic_field());
                               })
        .def_property_readonly("gravity", [](const RecordIMUFrame& frame) {
            py::module_ glm{py::module_::import("glm")};
            return create_py_vec3(glm, frame.gravity());
        });

    py::class_<RecordPoseFrame, RecordFrame>(m, "RecordPoseFrame")
        .def(py::init([](int64_t time_point_us,
                         const py::object& py_translation,
                         const py::object& py_rotation) {
            glm::vec3 translation{read_py_vec3(py_translation)};
            glm::quat rotation{read_py_quat(py_rotation)};
            return RecordPoseFrame{time_point_us, translation, rotation};
        }))
        .def_property_readonly("time_point_us", &RecordPoseFrame::time_point_us)
        .def_property_readonly(
            "translation", [](const RecordPoseFrame& frame) {
                py::module_ glm{py::module_::import("glm")};
                return create_py_vec3(glm, frame.translation());
            })
        .def_property_readonly(
            "rotation", [](const RecordPoseFrame& frame) {
                py::module_ glm{py::module_::import("glm")};
                return create_py_quat(glm, frame.rotation());
            });

    py::class_<RecordCalibrationFrame, RecordFrame>(m, "RecordCalibrationFrame")
        .def(py::init<int64_t, shared_ptr<CameraCalibration>>())
        .def_property_readonly("time_point_us", &RecordCalibrationFrame::time_point_us)
        .def_property_readonly(
            "camera_calibration", [](const RecordCalibrationFrame& frame) {
                return frame.camera_calibration();
            });

    py::class_<Record>(m, "Record")
        .def("get_offsets", &Record::offsets, py::return_value_policy::copy)
        .def("get_info", &Record::info, py::return_value_policy::copy)
        .def("get_tracks", &Record::tracks, py::return_value_policy::copy)
        .def("get_attachments", &Record::attachments, py::return_value_policy::copy)
        .def("get_video_frames", &Record::video_frames, py::return_value_policy::copy)
        .def("get_audio_frames", &Record::audio_frames, py::return_value_policy::copy)
        .def("get_imu_frames", &Record::imu_frames, py::return_value_policy::copy)
        .def("get_pose_frames", &Record::pose_frames, py::return_value_policy::copy)
        .def("get_calibration_frames", &Record::calibration_frames, py::return_value_policy::copy)
        .def("get_direction_table", &Record::direction_table, py::return_value_policy::copy);
    // END record.hpp

    // BEGIN record_bytes_builder.hpp
    py::class_<RecordBytesBuilder>(m, "RecordBytesBuilder")
        .def(py::init<>())
        .def("set_sample_rate", &RecordBytesBuilder::setSampleRate)
        .def("set_depth_codec_type", &RecordBytesBuilder::setDepthCodecType)
        .def("set_depth_unit", &RecordBytesBuilder::setDepthUnit)
        .def("set_calibration", &RecordBytesBuilder::setCalibration)
        .def("set_cover_png_bytes", &RecordBytesBuilder::setCoverPNGBytes)
        .def("add_video_frame", &RecordBytesBuilder::addVideoFrame)
        .def("add_audio_frame", &RecordBytesBuilder::addAudioFrame)
        .def("add_imu_frame", &RecordBytesBuilder::addIMUFrame)
        .def("add_pose_frame", &RecordBytesBuilder::addPoseFrame)
        .def("add_calibration_frame", &RecordBytesBuilder::addCalibrationFrame)
        .def("build", &RecordBytesBuilder::build)
        .def("build_to_path", &RecordBytesBuilder::buildToPath);
    // END record_bytes_builder.hpp

    // BEGIN record_parser.hpp
    py::class_<RecordParser>(m, "RecordParser")
        .def(py::init<const string&>())
        .def("parse", &RecordParser::parse);
    // END record_parser.hpp

    // BEGIN frame_mapper.hpp
    py::class_<FrameMapper>(m, "FrameMapper")
        .def(py::init<const rgbd::CameraCalibration&, const rgbd::CameraCalibration&>())
        .def("map_color_frame", &FrameMapper::mapColorFrame)
        .def("map_depth_frame", &FrameMapper::mapDepthFrame);
    // END frame_mapper.hpp

    // BEGIN integer_frame.hpp
    py::class_<Int32Frame>(m, "Int32Frame")
        .def(py::init([](const py::array_t<int32_t> array) {
            py::buffer_info buffer{array.request()};
            int width{gsl::narrow<int>(buffer.shape[1])};
            int height{gsl::narrow<int>(buffer.shape[0])};
            return Int32Frame{width, height, static_cast<int32_t*>(buffer.ptr)};
        }))
        .def_property_readonly("width", &Int32Frame::width)
        .def_property_readonly("height", &Int32Frame::height)
        .def("get_values", [](const Int32Frame& frame) {
            py::array_t<int32_t> array(frame.values().size(), frame.values().data());
            array = array.reshape({frame.height(), frame.width()});
            return array;
        })
        .def(py::pickle(
            [](const Int32Frame& frame) { // dump
                return py::make_tuple(frame.width(), frame.height(), frame.values());
            },
            [](py::tuple t) { // load
                int width{t[0].cast<int>()};
                int height{t[1].cast<int>()};
                auto values{t[2].cast<vector<int32_t>>()};

                return Int32Frame{width, height, values};
            }
        ));
    // END integer_frame.hpp

    // BEGIN ios_camera_distortion.hpp
    py::class_<IosCameraCalibration,
               CameraCalibration,
               std::shared_ptr<IosCameraCalibration>>(m, "IosCameraCalibration")
        .def(py::init<int, int, int, int, float, float, float, float, float, float, float, float, span<const float>, span<const float>>())
        .def_property_readonly("fx", &IosCameraCalibration::fx)
        .def_property_readonly("fy", &IosCameraCalibration::fy)
        .def_property_readonly("ox", &IosCameraCalibration::ox)
        .def_property_readonly("oy", &IosCameraCalibration::oy)
        .def_property_readonly("reference_dimension_width", &IosCameraCalibration::reference_dimension_width)
        .def_property_readonly("reference_dimension_height", &IosCameraCalibration::reference_dimension_height)
        .def_property_readonly("lens_distortion_center_x", &IosCameraCalibration::lens_distortion_center_x)
        .def_property_readonly("lens_distortion_center_y", &IosCameraCalibration::lens_distortion_center_y);
    // END undistorted_camera_distortion.hpp

    // BEGIN math_utils.hpp
    py::class_<MathUtils>(m, "MathUtils")
        .def_static("apply_rotation_rate_and_gravity_to_rotation",
                    [](const py::object& py_rotation,
                       float delta_time_sec,
                       const py::object& py_rotation_rate,
                       const py::object& py_gravity) {
                        auto rotation{read_py_quat(py_rotation)};
                        auto rotation_rate{read_py_vec3(py_rotation_rate)};
                        auto gravity{read_py_vec3(py_gravity)};
                        auto new_rotation{MathUtils::applyRotationRateAndGravityToRotation(
                            rotation, delta_time_sec, rotation_rate, gravity)};
                        py::module_ glm{py::module_::import("glm")};

                        return glm.attr("quat")(
                            new_rotation.w, new_rotation.x, new_rotation.y, new_rotation.z);
                    })
        .def_static("convert_euler_angles_to_quaternion",
                    [](const py::object& py_euler_angles) {
                        auto euler_angles{read_py_vec3(py_euler_angles)};
                        auto quat{MathUtils::convertEulerAnglesToQuaternion(euler_angles)};
                        py::module_ glm{py::module_::import("glm")};
                        return glm.attr("quat")(quat.w, quat.x, quat.y, quat.z);
                    })
        .def_static("compute_gravity_compensating_euler_angles",
                    [](const py::object& py_gravity) {
                        auto gravity{read_py_vec3(py_gravity)};
                        auto euler_angles{MathUtils::computeGravityCompensatingEulerAngles(gravity)};
                        py::module_ glm{py::module_::import("glm")};
                        return glm.attr("vec3")(euler_angles.x, euler_angles.y, euler_angles.z);
                    });


                    
    // END math_utils.hpp

    // BEGIN undistorted_camera_distortion.hpp
    py::class_<UndistortedCameraCalibration,
               CameraCalibration,
               std::shared_ptr<UndistortedCameraCalibration>>(m, "UndistortedCameraCalibration")
        .def(py::init<int, int, int, int, float, float, float, float>())
        .def_property_readonly("fx", &UndistortedCameraCalibration::fx)
        .def_property_readonly("fy", &UndistortedCameraCalibration::fy)
        .def_property_readonly("cx", &UndistortedCameraCalibration::cx)
        .def_property_readonly("cy", &UndistortedCameraCalibration::cy);
    // END undistorted_camera_distortion.hpp

    // BEGIN yuv_frame.hpp
    py::class_<YuvFrame>(m, "YuvFrame")
        .def(py::init([](const py::array_t<uint8_t> y_array,
                         const py::array_t<uint8_t> u_array,
                         const py::array_t<uint8_t> v_array) {
            py::buffer_info y_buffer{y_array.request()};
            py::buffer_info u_buffer{u_array.request()};
            py::buffer_info v_buffer{v_array.request()};
            int width{gsl::narrow<int>(y_buffer.shape[1])};
            int height{gsl::narrow<int>(y_buffer.shape[0])};
            return YuvFrame{width,
                            height,
                            static_cast<uint8_t*>(y_buffer.ptr),
                            static_cast<uint8_t*>(u_buffer.ptr),
                            static_cast<uint8_t*>(v_buffer.ptr)};
        }))
        .def_property_readonly("width", &YuvFrame::width)
        .def_property_readonly("height", &YuvFrame::height)
        .def("get_y_channel",
             [](const YuvFrame& frame) {
                 py::array_t<uint8_t> y_array(frame.y_channel().size(), frame.y_channel().data());
                 y_array = y_array.reshape({frame.height(), frame.width()});
                 return y_array;
             })
        .def("get_u_channel",
             [](const YuvFrame& frame) {
                 py::array_t<uint8_t> u_array(frame.u_channel().size(), frame.u_channel().data());
                 u_array = u_array.reshape({frame.height() / 2, frame.width() / 2});
                 return u_array;
             })
        .def("get_v_channel",
             [](const YuvFrame& frame) {
                 py::array_t<uint8_t> v_array(frame.v_channel().size(), frame.v_channel().data());
                 v_array = v_array.reshape({frame.height() / 2, frame.width() / 2});
                 return v_array;
             })
        .def("get_mkv_cover_sized",
             [](const YuvFrame& frame) {
                 auto mkv_cover_sized{frame.getMkvCoverSized()};
                 return YuvFrame{std::move(*mkv_cover_sized)};
             })
        .def("get_png_bytes", [](const YuvFrame& frame) { return frame.getPNGBytes(); })
        .def(py::pickle(
            [](const YuvFrame& frame) { // dump
                return py::make_tuple(frame.width(), frame.height(),
                                      frame.y_channel(),
                                      frame.u_channel(),
                                      frame.v_channel());
            },
            [](py::tuple t) { // load
                int width{t[0].cast<int>()};
                int height{t[1].cast<int>()};
                auto y_channel{t[2].cast<vector<uint8_t>>()};
                auto u_channel{t[3].cast<vector<uint8_t>>()};
                auto v_channel{t[4].cast<vector<uint8_t>>()};

                return YuvFrame{width, height,
                                y_channel,
                                u_channel,
                                u_channel};
            }
        ));
    // END yuv_frame.hpp
}
