#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
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
        .def_property_readonly("camera_device_type", &CameraCalibration::getCameraDeviceType)
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
        .def(py::init<ColorCodecType, int, int, int>())
        .def("encode", &ColorEncoder::encode);
    // END color_encoder.hpp

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
        .def_static("create_rvl_encoder", &DepthEncoder::createRVLEncoder)
        .def_static("create_tdc1_encoder", &DepthEncoder::createTDC1Encoder)
        .def_property_readonly("codec_type", &DepthEncoder::getCodecType)
        .def("encode",
             [](DepthEncoder& encoder, const py::array_t<int32_t> depth_array, bool keyframe) {
                 py::buffer_info depth_buffer{depth_array.request()};
                 return encoder.encode(static_cast<int32_t*>(depth_buffer.ptr), keyframe);
             });
    // END depth_encoder.hpp

    // BEGIN constants.hpp
    py::enum_<CameraDeviceType>(m, "CameraDeviceType")
        .value("AzureKinect", CameraDeviceType::AzureKinect)
        .value("IOS", CameraDeviceType::IOS)
        .value("Undistorted", CameraDeviceType::Undistorted);

    py::enum_<ColorCodecType>(m, "ColorCodecType").value("VP8", ColorCodecType::VP8);

    py::enum_<DepthCodecType>(m, "DepthCodecType")
        .value("RVL", DepthCodecType::RVL)
        .value("TDC1", DepthCodecType::TDC1);

    m.attr("AUDIO_SAMPLE_RATE") = AUDIO_SAMPLE_RATE;
    m.attr("AUDIO_INPUT_SAMPLES_PER_FRAME") = AUDIO_INPUT_SAMPLES_PER_FRAME;

    // END constants.hpp

    // BEGIN file.hpp
    py::class_<FileOffsets>(m, "FileOffsets")
        .def(py::init())
        .def_readwrite("segment_info_offset", &FileOffsets::segment_info_offset)
        .def_readwrite("tracks_offset", &FileOffsets::tracks_offset)
        .def_readwrite("attachments_offset", &FileOffsets::attachments_offset)
        .def_readwrite("first_cluster_offset", &FileOffsets::first_cluster_offset);

    py::class_<FileInfo>(m, "FileInfo")
        .def(py::init())
        .def_readwrite("timecode_scale_ns", &FileInfo::timecode_scale_ns)
        .def_readwrite("duration_us", &FileInfo::duration_us)
        .def_readwrite("writing_app", &FileInfo::writing_app);

    py::class_<FileVideoTrack>(m, "FileVideoTrack")
        .def(py::init())
        .def_readwrite("track_number", &FileVideoTrack::track_number)
        .def_readwrite("default_duration_ns", &FileVideoTrack::default_duration_ns)
        .def_readwrite("width", &FileVideoTrack::width)
        .def_readwrite("height", &FileVideoTrack::height);

    py::class_<FileColorVideoTrack, FileVideoTrack>(m, "FileColorVideoTrack")
        .def(py::init())
        .def_readwrite("codec", &FileColorVideoTrack::codec);

    py::class_<FileDepthVideoTrack, FileVideoTrack>(m, "FileDepthVideoTrack")
        .def(py::init())
        .def_readwrite("codec", &FileDepthVideoTrack::codec)
        .def_readwrite("depth_unit", &FileDepthVideoTrack::depth_unit);

    py::class_<FileAudioTrack>(m, "FileAudioTrack")
        .def(py::init())
        .def_readwrite("track_number", &FileAudioTrack::track_number)
        .def_readwrite("sampling_frequency", &FileAudioTrack::sampling_frequency);

    py::class_<FileTracks>(m, "FileTracks")
        .def(py::init())
        .def_readwrite("color_track", &FileTracks::color_track)
        .def_readwrite("depth_track", &FileTracks::depth_track)
        .def_readwrite("audio_track", &FileTracks::audio_track)
        .def_readwrite("floor_track_number", &FileTracks::floor_track_number)
        .def_readwrite("acceleration_track_number", &FileTracks::acceleration_track_number)
        .def_readwrite("rotation_rate_track_number", &FileTracks::rotation_rate_track_number)
        .def_readwrite("magnetic_field_track_number", &FileTracks::magnetic_field_track_number)
        .def_readwrite("gravity_track_number", &FileTracks::gravity_track_number)
        .def_readwrite("translation_track_number", &FileTracks::translation_track_number)
        .def_readwrite("rotation_track_number", &FileTracks::rotation_track_number)
        .def_readwrite("scale_track_number", &FileTracks::scale_track_number);

    py::class_<FileAttachments>(m, "FileAttachments")
        .def(py::init())
        .def_readwrite("camera_calibration", &FileAttachments::camera_calibration)
        .def_readwrite("cover_png_bytes", &FileAttachments::cover_png_bytes);

    py::enum_<FileFrameType>(m, "FileFrameType")
        .value("Video", FileFrameType::Video)
        .value("Audio", FileFrameType::Audio)
        .value("IMU", FileFrameType::IMU)
        .value("TRS", FileFrameType::TRS);

    py::class_<FileFrame>(m, "FileFrame").def("getType", &FileFrame::getType);

    py::class_<FileVideoFrame, FileFrame>(m, "FileVideoFrame")
        .def(py::init<int64_t, bool, const Bytes&, const Bytes&>())
        .def_property_readonly("time_point_us", &FileVideoFrame::time_point_us)
        .def_property_readonly("keyframe", &FileVideoFrame::keyframe)
        .def_property_readonly("color_bytes", &FileVideoFrame::color_bytes)
        .def_property_readonly("depth_bytes", &FileVideoFrame::depth_bytes);

    py::class_<FileAudioFrame, FileFrame>(m, "FileAudioFrame")
        .def(py::init<int64_t, const Bytes&>())
        .def_property_readonly("time_point_us", &FileAudioFrame::time_point_us)
        .def_property_readonly("bytes", &FileAudioFrame::bytes);

    py::class_<FileIMUFrame, FileFrame>(m, "FileIMUFrame")
        .def(py::init<int64_t,
                      const glm::vec3&,
                      const glm::vec3&,
                      const glm::vec3&,
                      const glm::vec3&>())
        .def_property_readonly("time_point_us", &FileIMUFrame::time_point_us)
        .def_property_readonly("acceleration",
                               [](const FileIMUFrame& frame) {
                                   py::module_ glm{py::module_::import("glm")};
                                   return create_py_vec3(glm, frame.acceleration());
                               })
        .def_property_readonly("rotation_rate",
                               [](const FileIMUFrame& frame) {
                                   py::module_ glm{py::module_::import("glm")};
                                   return create_py_vec3(glm, frame.rotation_rate());
                               })
        .def_property_readonly("magnetic_field",
                               [](const FileIMUFrame& frame) {
                                   py::module_ glm{py::module_::import("glm")};
                                   return create_py_vec3(glm, frame.magnetic_field());
                               })
        .def_property_readonly("gravity", [](const FileIMUFrame& frame) {
            py::module_ glm{py::module_::import("glm")};
            return create_py_vec3(glm, frame.gravity());
        });

    py::class_<FileTRSFrame, FileFrame>(m, "FileTRSFrame")
        .def(py::init([](int64_t time_point_us,
                         const py::object& py_translation,
                         const py::object& py_rotation,
                         const py::object& py_scale) {
            glm::vec3 translation{read_py_vec3(py_translation)};
            glm::quat rotation{read_py_quat(py_rotation)};
            glm::vec3 scale{read_py_vec3(py_scale)};
            return FileTRSFrame{time_point_us, translation, rotation, scale};
        }))
        .def_property_readonly("time_point_us", &FileTRSFrame::time_point_us)
        .def_property_readonly("translation", &FileTRSFrame::translation)
        .def_property_readonly("rotation", &FileTRSFrame::rotation)
        .def_property_readonly("scale", &FileTRSFrame::scale);

    py::class_<File>(m, "File")
        .def_property_readonly("offsets", &File::offsets, py::return_value_policy::copy)
        .def_property_readonly("info", &File::info, py::return_value_policy::copy)
        .def_property_readonly("tracks", &File::tracks, py::return_value_policy::copy)
        .def_property_readonly("attachments", &File::attachments, py::return_value_policy::copy)
        .def_property_readonly("video_frames", &File::video_frames, py::return_value_policy::copy)
        .def_property_readonly("audio_frames", &File::audio_frames, py::return_value_policy::copy)
        .def_property_readonly("imu_frames", &File::imu_frames, py::return_value_policy::copy)
        .def_property_readonly("trs_frames", &File::trs_frames, py::return_value_policy::copy)
        .def_property_readonly("direction_table", &File::direction_table, py::return_value_policy::copy);
    // END file.hpp

    // BEGIN file_bytes_builder.hpp
    py::class_<FileBytesBuilder>(m, "FileBytesBuilder")
        .def(py::init<>())
        .def("set_framerate", &FileBytesBuilder::setFramerate)
        .def("set_samplerate", &FileBytesBuilder::setSamplerate)
        .def("set_depth_codec_type", &FileBytesBuilder::setDepthCodecType)
        .def("set_depth_unit", &FileBytesBuilder::setDepthUnit)
        .def("set_calibration", &FileBytesBuilder::setCalibration)
        .def("set_cover_png_bytes", &FileBytesBuilder::setCoverPNGBytes)
        .def("add_video_frame", &FileBytesBuilder::addVideoFrame)
        .def("add_audio_frame", &FileBytesBuilder::addAudioFrame)
        .def("add_imu_frame", &FileBytesBuilder::addIMUFrame)
        .def("add_trs_frame", &FileBytesBuilder::addTRSFrame)
        .def("build", &FileBytesBuilder::build)
        .def("build_to_path", &FileBytesBuilder::buildToPath);
    // END file_bytes_builder.hpp

    // BEGIN file_parser.hpp
    py::class_<FileParser>(m, "FileParser")
        .def(py::init<const string&>())
        .def("parse", &FileParser::parse);
    // END file_parser.hpp

    // BEGIN frame_mapper.hpp
    py::class_<FrameMapper>(m, "FrameMapper")
        .def(py::init<const rgbd::CameraCalibration&, const rgbd::CameraCalibration&>())
        .def("map_color_frame", &FrameMapper::mapColorFrame)
        .def("map_depth_frame", &FrameMapper::mapDepthFrame);
    // END frame_mapper.hpp

    // BEGIN integer_frame.hpp
    py::class_<Int32Frame>(m, "Int32Frame")
        .def_property_readonly("width", &Int32Frame::width)
        .def_property_readonly("height", &Int32Frame::height)
        .def_property_readonly("values", [](const Int32Frame& frame) {
            py::array_t<int32_t> array(frame.values().size(), frame.values().data());
            array = array.reshape({frame.height(), frame.width()});
            return array;
        });
    // END integer_frame.hpp

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
                    });
    // END math_utils.hpp

    // BEGIN undistorted_camera_distortion.hpp
    py::class_<UndistortedCameraCalibration, CameraCalibration, std::shared_ptr<UndistortedCameraCalibration>>(m, "UndistortedCameraCalibration")
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
                int width{y_buffer.shape[1]};
                int height{y_buffer.shape[0]};
                return YuvFrame{width,
                                height,
                                static_cast<uint8_t*>(y_buffer.ptr),
                                static_cast<uint8_t*>(u_buffer.ptr),
                                static_cast<uint8_t*>(v_buffer.ptr)};
            })
        )
        .def_property_readonly("width", &YuvFrame::width)
        .def_property_readonly("height", &YuvFrame::height)
        .def_property_readonly("y_channel", [](const YuvFrame& frame) {
            py::array_t<uint8_t> y_array(frame.y_channel().size(), frame.y_channel().data());
            y_array = y_array.reshape({frame.height(), frame.width()});
            return y_array;
        })
        .def_property_readonly("u_channel", [](const YuvFrame& frame) {
            py::array_t<uint8_t> u_array(frame.u_channel().size(), frame.u_channel().data());
            u_array = u_array.reshape({frame.height() / 2, frame.width() / 2});
            return u_array;
        })
        .def_property_readonly("v_channel", [](const YuvFrame& frame) {
            py::array_t<uint8_t> v_array(frame.v_channel().size(), frame.v_channel().data());
            v_array = v_array.reshape({frame.height() / 2, frame.width() / 2});
            return v_array;
        })
        .def("get_mkv_cover_sized", [](const YuvFrame& frame) {
            auto mkv_cover_sized{frame.getMkvCoverSized()};
            return YuvFrame{std::move(*mkv_cover_sized)};
        })
        .def("get_png_bytes", [](const YuvFrame& frame) {
            return frame.getPNGBytes();
        });
    // END yuv_frame.hpp
}
