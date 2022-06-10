cmake_minimum_required(VERSION 3.16)
project(librgbd)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

add_subdirectory(deps/libebml EXCLUDE_FROM_ALL)
# libmatroska consumes libebml as EBML::ebml
add_library(EBML::ebml ALIAS ebml)
add_subdirectory(deps/libmatroska EXCLUDE_FROM_ALL)
set_target_properties(ebml PROPERTIES
  FOLDER "Dependencies"
)
set_target_properties(matroska PROPERTIES FOLDER "Dependencies")

add_library(rgbd
  librgbd.cpp
)
target_include_directories(rgbd PUBLIC ${PROJECT_BINARY_DIR}/deps/libebml)
target_link_libraries(rgbd PUBLIC matroska)

message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")