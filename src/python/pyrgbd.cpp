#include <pybind11/pybind11.h>
#include <rgbd/rgbd.hpp>

namespace py = pybind11;

PYBIND11_MODULE(pyrgbd, m)
{
    m.doc() = "Library for reading and writing RGBD videos.";

    py::class_<rgbd::AudioFrame>(m, "AudioFrame")
        .def("time_point_us", &rgbd::AudioFrame::time_point_us);
}
