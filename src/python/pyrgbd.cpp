#include <pybind11/pybind11.h>
#include <rgbd/rgbd.hpp>

namespace py = pybind11;

PYBIND11_MODULE(pyrgbd, m)
{
    m.doc() = "Library for reading and writing RGBD videos.";

    // BEGIN file.hpp
    py::class_<rgbd::FileOffsets>(m, "FileOffsets")
        .def(py::init())
        .def_readwrite("segment_info_offset", &rgbd::FileOffsets::segment_info_offset)
        .def_readwrite("tracks_offset", &rgbd::FileOffsets::tracks_offset)
        .def_readwrite("attachments_offset", &rgbd::FileOffsets::attachments_offset)
        .def_readwrite("first_cluster_offset", &rgbd::FileOffsets::first_cluster_offset);

    py::class_<rgbd::FileInfo>(m, "FileInfo")
        .def(py::init())
        .def_readwrite("timecode_scale_ns", &rgbd::FileInfo::timecode_scale_ns)
        .def_readwrite("duration_us", &rgbd::FileInfo::duration_us)
        .def_readwrite("writing_app", &rgbd::FileInfo::writing_app);

    py::class_<rgbd::FileVideoTrack>(m, "FileVideoTrack")
        .def(py::init())
        .def_readwrite("track_number", &rgbd::FileVideoTrack::track_number)
        .def_readwrite("default_duration_ns", &rgbd::FileVideoTrack::default_duration_ns)
        .def_readwrite("width", &rgbd::FileVideoTrack::width)
        .def_readwrite("height", &rgbd::FileVideoTrack::height);

    py::class_<rgbd::FileColorVideoTrack, rgbd::FileVideoTrack>(m, "FileColorVideoTrack")
        .def(py::init())
        .def_readwrite("codec", &rgbd::FileColorVideoTrack::codec);

    py::class_<rgbd::FileDepthVideoTrack, rgbd::FileVideoTrack>(m, "FileDepthVideoTrack")
        .def(py::init())
        .def_readwrite("codec", &rgbd::FileDepthVideoTrack::codec)
        .def_readwrite("depth_unit", &rgbd::FileDepthVideoTrack::depth_unit);
    // END file.hpp


    py::class_<rgbd::FileParser>(m, "FileParser")
        .def("parse", &rgbd::FileParser::parse);
}
