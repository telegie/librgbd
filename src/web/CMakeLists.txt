add_executable(rgbd-wasm rgbd_wasm.cpp)
target_link_libraries(rgbd-wasm PRIVATE rgbd-static)
set_target_properties(rgbd-wasm PROPERTIES CXX_STANDARD 17)
