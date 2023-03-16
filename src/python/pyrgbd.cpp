#include <pybind11/pybind11.h>
#include <rgbd/rgbd.hpp>

namespace py = pybind11;
using namespace rgbd;

PYBIND11_MODULE(pyrgbd, m)
{
    m.doc() = "Library for reading and writing RGBD videos.";

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

    py::class_<FileFrame>(m, "FileFrame")
        .def("getType", &FileFrame::getType);

    py::class_<FileVideoFrame, FileFrame>(m, "FileVideoFrame")
        .def_property_readonly("time_point_us", &FileVideoFrame::time_point_us)
        .def_property_readonly("keyframe", &FileVideoFrame::keyframe)
        .def_property_readonly("color_bytes", &FileVideoFrame::color_bytes)
        .def_property_readonly("depth_bytes", &FileVideoFrame::depth_bytes);

    py::class_<FileIMUFrame, FileFrame>(m, "FileIMUFrame")
        .def_property_readonly("time_point_us", &FileIMUFrame::time_point_us)
        .def_property_readonly("acceleration", &FileIMUFrame::acceleration)
        .def_property_readonly("rotation_rate", &FileIMUFrame::rotation_rate)
        .def_property_readonly("magnetic_field", &FileIMUFrame::magnetic_field)
        .def_property_readonly("gravity", &FileIMUFrame::gravity);

    py::class_<FileTRSFrame, FileFrame>(m, "FileIMUFrame")
        .def_property_readonly("time_point_us", &FileTRSFrame::time_point_us)
        .def_property_readonly("translation", &FileTRSFrame::translation)
        .def_property_readonly("rotation", &FileTRSFrame::rotation)
        .def_property_readonly("scale", &FileTRSFrame::scale);

    py::class_<File>(m, "File")
        .def_property_readonly("offsets", &File::offsets)
        .def_property_readonly("info", &File::info)
        .def_property_readonly("tracks", &File::tracks)
        .def_property_readonly("attachments", &File::attachments)
        .def_property_readonly("video_frames", &File::video_frames)
        .def_property_readonly("audio_frames", &File::audio_frames)
        .def_property_readonly("imu_frames", &File::imu_frames)
        .def_property_readonly("trs_frames", &File::trs_frames)
        .def_property_readonly("direction_table", &File::direction_table);
    // END file.hpp

    // BEGIN file_parser.hpp
    py::class_<FileParser>(m, "FileParser")
        .def("parse", &FileParser::parse);
    // END file_parser.hpp
}
