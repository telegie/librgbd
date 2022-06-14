cmake_minimum_required(VERSION 3.16)
project(librgbd)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

### FFMPEG BEGIN ###
set(FFMPEG_BINARIES_DIR ${PROJECT_SOURCE_DIR}/deps/ffmpeg-binaries)
set(FFMPEG_WIN64_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/x64-windows)
set(FFMPEG_MAC_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/arm64-mac)
set(FFMPEG_WASM_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/wasm32-emscripten)
set(FFMPEG_IOS_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/arm64-ios)
### FFMPEG END ###

### LIBMATROSKA BEGIN ###
add_subdirectory(deps/libebml EXCLUDE_FROM_ALL)
# libmatroska consumes libebml as EBML::ebml
add_library(EBML::ebml ALIAS ebml)
add_subdirectory(deps/libmatroska EXCLUDE_FROM_ALL)
set_target_properties(ebml PROPERTIES
  FOLDER "Dependencies"
)
set_target_properties(matroska PROPERTIES FOLDER "Dependencies")
### LIBMATROSKA END ###

### GSL BEGIN ###
# GSL is header only.
set(GSL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/GSL/include)
### GSL END ###

### SPDLOG BEGIN ###
add_subdirectory(deps/spdlog EXCLUDE_FROM_ALL)
### SPDLOG END ###

add_library(rgbd
  include/rgbd/constants.hpp
  include/rgbd/ffmpeg_audio_decoder.hpp
  include/rgbd/ffmpeg_audio_encoder.hpp
  include/rgbd/ffmpeg_video_decoder.hpp
  include/rgbd/ffmpeg_video_encoder.hpp
  include/rgbd/ffmpeg_utils.hpp
  include/rgbd/yuv_frame.hpp
  src/constants.cpp
  src/ffmpeg_audio_encoder.cpp
  src/ffmpeg_audio_decoder.cpp
  src/ffmpeg_video_decoder.cpp
  src/ffmpeg_video_encoder.cpp
  src/ffmpeg_utils.cpp
  src/yuv_frame.cpp
)
set(RgbdIncludeDirectories ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/deps/libebml ${GSL_INCLUDE_DIR})
set(RgbdLibraryDependencies matroska spdlog::spdlog)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_WIN64_DIR}/include
  )
  list(APPEND RgbdLibraryDependencies
    ${FFMPEG_WIN64_DIR}/bin/avcodec.lib
    ${FFMPEG_WIN64_DIR}/bin/avformat.lib
    ${FFMPEG_WIN64_DIR}/bin/avutil.lib
  )
  # Set RGBD_FFMPEG_BIN in parent scope for parent project to copy the dll files.
  set(RGBD_FFMPEG_BIN ${FFMPEG_WIN64_DIR}/bin PARENT_SCOPE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_MAC_DIR}/include
  )
  list(APPEND RgbdLibraryDependencies
    ${FFMPEG_MAC_DIR}/lib/libavcodec.a
    ${FFMPEG_MAC_DIR}/lib/libavformat.a
    ${FFMPEG_MAC_DIR}/lib/libavutil.a
    ${FFMPEG_BINARIES_DIR}/libvpx-binaries/1.10.0/arm64-mac/lib/libvpx.a
    ${FFMPEG_BINARIES_DIR}/opus-binaries/e4d4b74/arm64-mac/lib/libopus.a
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_IOS_DIR}/include
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_WASM_DIR}/include
  )
  list(APPEND RgbdLibraryDependencies
    ${FFMPEG_WASM_DIR}/lib/libavcodec.a
    ${FFMPEG_WASM_DIR}/lib/libavformat.a
    ${FFMPEG_WASM_DIR}/lib/libavutil.a
    ${FFMPEG_WASM_DIR}/lib/libswresample.a
    ${FFMPEG_BINARIES_DIR}/opus-binaries/e4d4b74/wasm32-emscripten/lib/libopus.a
  )
endif()

target_include_directories(rgbd PRIVATE ${PROJECT_SOURCE_DIR}/include/rgbd)
target_include_directories(rgbd PUBLIC ${RgbdIncludeDirectories})
target_link_libraries(rgbd PUBLIC ${RgbdLibraryDependencies})
set_target_properties(rgbd PROPERTIES CXX_STANDARD 17)
