pybind11_add_module(pyrgbd pyrgbd.cpp)
target_link_libraries(pyrgbd PUBLIC rgbd-static)
set_target_properties(pyrgbd PROPERTIES CXX_STANDARD 17)
