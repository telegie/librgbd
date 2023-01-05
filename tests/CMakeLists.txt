add_executable(rgbd-tests rgbd_tests.cpp)
target_link_libraries(rgbd-tests PUBLIC Catch2::Catch2WithMain rgbd-static)
set_target_properties(rgbd-tests PROPERTIES
  CXX_STANDARD 17
  FOLDER "Tests"
)

include(Catch)
catch_discover_tests(rgbd-tests)
