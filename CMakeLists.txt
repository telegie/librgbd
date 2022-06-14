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
set(LIBPNG_INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/deps/libpng
  ${CMAKE_CURRENT_BINARY_DIR}/deps/libpng # libpng generates header files here that should be included in applications using it.
)

# libpng depends on zlib.
set(ZLIB_BINARIES_DIR ${PROJECT_SOURCE_DIR}/deps/zlib-binaries)

if(TG_PLATFORM_WINDOWS)
  set(ZLIB_ROOT ${ZLIB_BINARIES_DIR}/1.2.11/x64-windows/release)
elseif(TG_PLATFORM_MAC)
  set(ZLIB_ROOT ${ZLIB_BINARIES_DIR}/1.2.11/arm64-mac)
endif()

# iOS build can have multiple targets set to CMAKE_OSX_ARCHITECTURES.
# This leads to CMAKE_SYSTEM_PROCESSOR not being set.
# However, in practice, it is okay to set CMAKE_SYSTEM_PROCESSOR to arm
# as all possible architectures of iOS are arm-based.
# Setting CMAKE_SYSTEM_PROCESSOR is required for libpng build,
# which changes its behavior based on CMAKE_SYSTEM_PROCESSOR.
if(TG_PLATFORM_IOS AND CMAKE_SYSTEM_PROCESSOR STREQUAL "")
  message("Setting empty CMAKE_SYSTEM_PROCESSOR to arm")
  set(CMAKE_SYSTEM_PROCESSOR arm)
endif()

# Install libpng and its dependency, zlib.
if(TG_PLATFORM_WINDOWS OR TG_PLATFORM_MAC OR TG_PLATFORM_IOS)
  set(SKIP_INSTALL_ALL ON)
  if(TG_PLATFORM_MAC OR TG_PLATFORM_IOS)
    set(PNG_ARM_NEON off) # turned on due to checking causing compile errors
  endif()
  add_subdirectory(deps/libpng EXCLUDE_FROM_ALL)
  set_target_properties(genfiles PROPERTIES FOLDER "Dependencies")
  set_target_properties(png PROPERTIES FOLDER "Dependencies")

  set_target_properties(pngfix PROPERTIES FOLDER "Excluded")
  set_target_properties(png-fix-itxt PROPERTIES FOLDER "Excluded")
  set_target_properties(pngimage PROPERTIES FOLDER "Excluded")
  set_target_properties(pngstest PROPERTIES FOLDER "Excluded")
  set_target_properties(pngvalid PROPERTIES FOLDER "Excluded")
  set_target_properties(pngtest PROPERTIES FOLDER "Excluded")
  set_target_properties(pngunknown PROPERTIES FOLDER "Excluded")
  set_target_properties(png_static PROPERTIES FOLDER "Dependencies")
  if(TG_PLATFORM_MAC)
    set_target_properties(genprebuilt PROPERTIES FOLDER "Excluded") # causes an error in Windows
  endif()
endif()
### LIBPNG END ###

### SPDLOG BEGIN ###
add_subdirectory(deps/spdlog EXCLUDE_FROM_ALL)
### SPDLOG END ###

add_library(rgbd
  include/rgbd/audio_frame.hpp
  include/rgbd/byte_utils.hpp
  include/rgbd/camera_calibration.hpp
  include/rgbd/constants.hpp
  include/rgbd/depth_decoder.hpp
  include/rgbd/depth_encoder.hpp
  include/rgbd/ffmpeg_audio_decoder.hpp
  include/rgbd/ffmpeg_audio_encoder.hpp
  include/rgbd/ffmpeg_video_decoder.hpp
  include/rgbd/ffmpeg_video_encoder.hpp
  include/rgbd/ffmpeg_utils.hpp
  include/rgbd/int16_frame.hpp
  include/rgbd/ios_calibration_utils.hpp
  include/rgbd/ios_camera_calibration.hpp
  include/rgbd/kinect_calibration_utils.hpp
  include/rgbd/kinect_camera_calibration.hpp
  include/rgbd/plane.hpp
  include/rgbd/png_utils.hpp
  include/rgbd/recorder.hpp
  include/rgbd/rgbd_frame.hpp
  include/rgbd/rvl.hpp
  include/rgbd/tdc1_decoder.hpp
  include/rgbd/tdc1_encoder.hpp
  include/rgbd/video.hpp
  include/rgbd/video_info.hpp
  include/rgbd/video_parser.hpp
  include/rgbd/yuv_frame.hpp
  src/audio_frame.cpp
  src/byte_utils.cpp
  src/camera_calibration.cpp
  src/constants.cpp
  src/depth_decoder.cpp
  src/depth_encoder.cpp
  src/ffmpeg_audio_encoder.cpp
  src/ffmpeg_audio_decoder.cpp
  src/ffmpeg_video_decoder.cpp
  src/ffmpeg_video_encoder.cpp
  src/ffmpeg_utils.cpp
  src/int16_frame.cpp
  src/ios_calibration_utils.cpp
  src/ios_camera_calibration.cpp
  src/kinect_calibration_utils.cpp
  src/kinect_camera_calibration.cpp
  src/plane.cpp
  src/png_utils.cpp
  src/recorder.cpp
  src/rgbd_frame.cpp
  src/rvl.cpp
  src/tdc1_decoder.cpp
  src/tdc1_encoder.cpp
  src/video.cpp
  src/video_info.cpp
  src/video_parser.cpp
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

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_WIN64_DIR}/include
    ${LIBPNG_INCLUDE_DIRS}
  )
  list(APPEND RgbdLibraryDependencies
    ${FFMPEG_WIN64_DIR}/bin/avcodec.lib
    ${FFMPEG_WIN64_DIR}/bin/avformat.lib
    ${FFMPEG_WIN64_DIR}/bin/avutil.lib
    png
  )
  # Set RGBD_FFMPEG_BIN in parent scope for parent project to copy the dll files.
  set(RGBD_FFMPEG_BIN ${FFMPEG_WIN64_DIR}/bin PARENT_SCOPE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_MAC_DIR}/include
    ${LIBPNG_INCLUDE_DIRS}
  )
  list(APPEND RgbdLibraryDependencies
    ${FFMPEG_MAC_DIR}/lib/libavcodec.a
    ${FFMPEG_MAC_DIR}/lib/libavformat.a
    ${FFMPEG_MAC_DIR}/lib/libavutil.a
    ${FFMPEG_BINARIES_DIR}/libvpx-binaries/1.10.0/arm64-mac/lib/libvpx.a
    ${FFMPEG_BINARIES_DIR}/opus-binaries/e4d4b74/arm64-mac/lib/libopus.a
    png_static
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_IOS_DIR}/include
    ${LIBPNG_INCLUDE_DIRS}
  )
  list(APPEND RgbdLibraryDependencies
    png_static
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

  set(CommonLibraryWasmOptions "SHELL:-s USE_LIBPNG=1")
  target_link_options(CommonLibrary PUBLIC ${CommonLibraryWasmOptions})
  target_compile_options(CommonLibrary PUBLIC ${CommonLibraryWasmOptions})
endif()

target_include_directories(rgbd PRIVATE ${PROJECT_SOURCE_DIR}/include/rgbd)
target_include_directories(rgbd PUBLIC ${RgbdIncludeDirectories})
target_link_libraries(rgbd PUBLIC ${RgbdLibraryDependencies})
set_target_properties(rgbd PROPERTIES CXX_STANDARD 17)
