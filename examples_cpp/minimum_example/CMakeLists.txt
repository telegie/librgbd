add_executable(MinimumExample
  minimum_example.cpp
)
target_link_libraries(MinimumExample PUBLIC
  rgbd-static
)
set_target_properties(MinimumExample PROPERTIES
  CXX_STANDARD 17
  FOLDER "Examples"
)
