add_executable(rgbd-cli
  rgbd_cli.cpp
)

target_link_libraries(rgbd-cli PUBLIC rgbd-static cli cxxopts)
set_target_properties(rgbd-cli PROPERTIES CXX_STANDARD 17)
