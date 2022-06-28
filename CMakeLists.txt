cmake_minimum_required(VERSION 3.16)
project(librgbd)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Variables for later use in conditions.
set(RGBD_PLATFORM_WINDOWS FALSE)
set(RGBD_PLATFORM_MAC FALSE)
set(RGBD_PLATFORM_IOS FALSE)
set(RGBD_PLATFORM_WASM FALSE)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(RGBD_PLATFORM_WINDOWS TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(RGBD_PLATFORM_MAC TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  set(RGBD_PLATFORM_IOS TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set(RGBD_PLATFORM_WASM TRUE)
endif()

### CXXOPTS BEGIN ###
add_subdirectory(deps/cxxopts)
### CXXOPTS END ###

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
set_target_properties(ebml PROPERTIES FOLDER "Dependencies")
set_target_properties(matroska PROPERTIES FOLDER "Dependencies")
### LIBMATROSKA END ###

### GLM BEGIN ###
# GLM does not have an /include directory and is header-only.
set(GLM_DIR ${PROJECT_SOURCE_DIR}/deps/glm)
### GLM END ###

### GSL BEGIN ###
# GSL is header only.
set(GSL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/GSL/include)
### GSL END ###

### JSON BEGIN ###
add_subdirectory(deps/json)
### JSON BEGIN ###

### LIBPNG BEGIN ###
set(LIBPNG_BINARIES_DIR ${PROJECT_SOURCE_DIR}/deps/libpng-binaries)
set(LIBPNG_WIN64_DIR ${LIBPNG_BINARIES_DIR}/1.6.38-ssrobins/x64-windows/release)
set(LIBPNG_MAC_DIR ${LIBPNG_BINARIES_DIR}/1.6.38-ssrobins/arm64-mac)
set(LIBPNG_IOS_DIR ${LIBPNG_BINARIES_DIR}/1.6.38-ssrobins/arm64-ios)
### LIBPNG END ###

### SPDLOG BEGIN ###
# Use existing spdlog if there already is an spdlog added.
if(NOT TARGET spdlog::spdlog)
  add_subdirectory(deps/spdlog EXCLUDE_FROM_ALL)
endif()
### SPDLOG END ###

add_library(rgbd
  include/rgbd/audio_frame.hpp
  include/rgbd/byte_utils.hpp
  include/rgbd/camera_calibration.hpp
  include/rgbd/capi_utils.hpp
  include/rgbd/constants.hpp
  include/rgbd/depth_decoder.hpp
  include/rgbd/depth_encoder.hpp
  include/rgbd/ffmpeg_audio_decoder.hpp
  include/rgbd/ffmpeg_audio_encoder.hpp
  include/rgbd/ffmpeg_video_decoder.hpp
  include/rgbd/ffmpeg_video_encoder.hpp
  include/rgbd/ffmpeg_utils.hpp
  include/rgbd/file.hpp
  include/rgbd/file_info.hpp
  include/rgbd/file_parser.hpp
  include/rgbd/frame.hpp
  include/rgbd/integer_frame.hpp
  include/rgbd/ios_calibration_utils.hpp
  include/rgbd/ios_camera_calibration.hpp
  include/rgbd/kinect_calibration_utils.hpp
  include/rgbd/kinect_camera_calibration.hpp
  include/rgbd/plane.hpp
  include/rgbd/png_utils.hpp
  include/rgbd/recorder.hpp
  include/rgbd/rgbd_capi.h
  include/rgbd/rvl.hpp
  include/rgbd/tdc1_decoder.hpp
  include/rgbd/tdc1_encoder.hpp
  include/rgbd/yuv_frame.hpp
  src/audio_frame.cpp
  src/byte_utils.cpp
  src/camera_calibration.cpp
  src/capi_utils.cpp
  src/constants.cpp
  src/depth_decoder.cpp
  src/depth_encoder.cpp
  src/ffmpeg_audio_encoder.cpp
  src/ffmpeg_audio_decoder.cpp
  src/ffmpeg_video_decoder.cpp
  src/ffmpeg_video_encoder.cpp
  src/ffmpeg_utils.cpp
  src/file.cpp
  src/file_info.cpp
  src/file_parser.cpp
  src/frame.cpp
  src/integer_frame.cpp
  src/ios_calibration_utils.cpp
  src/ios_camera_calibration.cpp
  src/kinect_calibration_utils.cpp
  src/kinect_camera_calibration.cpp
  src/plane.cpp
  src/png_utils.cpp
  src/recorder.cpp
  src/rgbd_capi.cpp
  src/rvl.cpp
  src/tdc1_decoder.cpp
  src/tdc1_encoder.cpp
  src/yuv_frame.cpp
)
set(RgbdIncludeDirectories
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/deps/libebml
  ${PROJECT_BINARY_DIR}/deps/libmatroska
  ${GLM_DIR}
  ${GSL_INCLUDE_DIR}
)
set(RgbdLibraryDependencies
  matroska
  nlohmann_json::nlohmann_json
  spdlog::spdlog
)

if(RGBD_PLATFORM_WINDOWS)
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_WIN64_DIR}/include
    ${LIBPNG_WIN64_DIR}/include
  )
  list(APPEND RgbdLibraryDependencies
    ${FFMPEG_WIN64_DIR}/bin/avcodec.lib
    ${FFMPEG_WIN64_DIR}/bin/avformat.lib
    ${FFMPEG_WIN64_DIR}/bin/avutil.lib
    ${LIBPNG_WIN64_DIR}/lib/libpng16_static.lib
  )
  # Set RGBD_FFMPEG_BIN in parent scope for parent project to copy the dll files.
  set(RGBD_FFMPEG_BIN ${FFMPEG_WIN64_DIR}/bin PARENT_SCOPE)
elseif(RGBD_PLATFORM_MAC)
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_MAC_DIR}/include
    ${LIBPNG_MAC_DIR}/include
  )
  list(APPEND RgbdLibraryDependencies
    ${FFMPEG_MAC_DIR}/lib/libavcodec.a
    ${FFMPEG_MAC_DIR}/lib/libavformat.a
    ${FFMPEG_MAC_DIR}/lib/libavutil.a
    ${FFMPEG_BINARIES_DIR}/libvpx-binaries/1.10.0/arm64-mac/lib/libvpx.a
    ${FFMPEG_BINARIES_DIR}/opus-binaries/e4d4b74/arm64-mac/lib/libopus.a
    ${LIBPNG_MAC_DIR}/lib/libpng.a
  )
elseif(RGBD_PLATFORM_IOS)
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_IOS_DIR}/include
    ${LIBPNG_IOS_DIR}/include
  )
elseif(RGBD_PLATFORM_WASM)
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

  set(RgbdLibraryWasmOptions "SHELL:-s USE_LIBPNG=1")
  target_link_options(rgbd PUBLIC ${RgbdLibraryWasmOptions})
  target_compile_options(rgbd PUBLIC ${RgbdLibraryWasmOptions})
endif()

target_include_directories(rgbd PRIVATE ${PROJECT_SOURCE_DIR}/include/rgbd)
target_include_directories(rgbd PUBLIC ${RgbdIncludeDirectories})
target_link_libraries(rgbd PUBLIC ${RgbdLibraryDependencies})
set_target_properties(rgbd PROPERTIES CXX_STANDARD 17)

if(RGBD_PLATFORM_WINDOWS OR RGBD_PLATFORM_MAC)
  add_subdirectory(src/cli)
endif()
