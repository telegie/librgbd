add_executable(rgbd-cli
  rgbd_cli.cpp
)

if(RGBD_OS_LINUX)
# Using rgbd-static resulted in errors when compling. Using the shared library as a quick fix.
  target_link_libraries(rgbd-cli PUBLIC rgbd-shared cli cxxopts)
else()
  target_link_libraries(rgbd-cli PUBLIC rgbd-static cli cxxopts)
endif()
set_target_properties(rgbd-cli PROPERTIES CXX_STANDARD 17)
