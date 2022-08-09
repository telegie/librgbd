cmake_minimum_required(VERSION 3.21)
project(librgbd)

set(RGBD_VERSION_MAJOR 1)
set(RGBD_VERSION_MINOR 2)
set(RGBD_VERSION_PATCH 0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)

# Variables for later use in conditions.
set(RGBD_OS_WINDOWS FALSE)
set(RGBD_OS_MAC FALSE)
set(RGBD_OS_LINUX FALSE)
set(RGBD_OS_IOS FALSE)
set(RGBD_OS_WASM FALSE)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(RGBD_OS_WINDOWS TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(RGBD_OS_MAC TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(RGBD_OS_LINUX TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  set(RGBD_OS_IOS TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set(RGBD_OS_WASM TRUE)
endif()

### CLI BEGIN ###
add_subdirectory(deps/cli)
### CLI END ###

### CXXOPTS BEGIN ###
add_subdirectory(deps/cxxopts)
### CXXOPTS END ###

### FFMPEG BEGIN ###
set(FFMPEG_BINARIES_DIR ${PROJECT_SOURCE_DIR}/deps/ffmpeg-binaries)
set(FFMPEG_WIN64_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/x64-windows)
set(FFMPEG_MAC_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/arm64-mac)
set(FFMPEG_LINUX_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/x64-linux)
set(FFMPEG_WASM_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/wasm32-emscripten)
set(FFMPEG_IOS_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/arm64-ios)
### FFMPEG END ###

### LIBMATROSKA BEGIN ###
add_subdirectory(deps/libebml EXCLUDE_FROM_ALL)
# libmatroska consumes libebml as EBML::ebml
add_library(EBML::ebml ALIAS ebml)
add_subdirectory(deps/libmatroska EXCLUDE_FROM_ALL)
if(RGBD_OS_LINUX)
  target_compile_options(ebml PUBLIC -fPIC)
endif()
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
if(NOT TARGET nlohmann_json::nlohmann_json)
  add_subdirectory(deps/json)
endif()
### JSON BEGIN ###

### LIBPNG BEGIN ###
set(LIBPNG_BINARIES_DIR ${PROJECT_SOURCE_DIR}/deps/libpng-binaries)
set(LIBPNG_WIN64_DIR ${LIBPNG_BINARIES_DIR}/1.6.38-ssrobins/x64-windows/release)
set(LIBPNG_MAC_DIR ${LIBPNG_BINARIES_DIR}/1.6.38-ssrobins/arm64-mac)
set(LIBPNG_LINUX_DIR ${LIBPNG_BINARIES_DIR}/1.6.38-ssrobins/x64-linux)
set(LIBPNG_IOS_DIR ${LIBPNG_BINARIES_DIR}/1.6.38-ssrobins/arm64-ios)
### LIBPNG END ###

### OPENCV BEGIN ###
set(OPENCV_BINARIES_DIR ${PROJECT_SOURCE_DIR}/deps/opencv-binaries)
if(TG_OS_WINDOWS)
  set(OPENCV_INCLUDE ${OPENCV_BINARIES_DIR}/4.5.2/x64-windows/include)
  set(OPENCV_LIB ${OPENCV_BINARIES_DIR}/4.5.2/x64-windows/x64/vc16/lib)
  set(OPENCV_BIN ${OPENCV_BINARIES_DIR}/4.5.2/x64-windows/x64/vc16/bin)
elseif(TG_OS_MAC)
  set(OPENCV_INCLUDE ${OPENCV_BINARIES_DIR}/4.5.2/arm64-mac/include/opencv4)
  set(OPENCV_LIB ${OPENCV_BINARIES_DIR}/4.5.2/arm64-mac/lib)
endif()
### OPENCV END ###

### SPDLOG BEGIN ###
# Use existing spdlog if there already is an spdlog added.
if(NOT TARGET spdlog::spdlog)
  add_subdirectory(deps/spdlog EXCLUDE_FROM_ALL)
  if(RGBD_OS_LINUX)
    target_compile_options(spdlog PUBLIC -fPIC)
  endif()
endif()
### SPDLOG END ###

### ZSTD BEGIN ###
if(RGBD_OS_WINDOWS OR RGBD_OS_MAC OR RGBD_OS_LINUX)
  set(ZSTD_DIR ${PROJECT_SOURCE_DIR}/deps/zstd)
  add_subdirectory(deps/zstd/build/cmake EXCLUDE_FROM_ALL)
  set_target_properties(libzstd_shared PROPERTIES FOLDER "Dependencies")
endif()
### ZSTD END ###
set(RgbdSources
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
  include/rgbd/rgbd.hpp
  include/rgbd/rgbd_capi.h
  include/rgbd/rvl.hpp
  include/rgbd/tdc1_decoder.hpp
  include/rgbd/tdc1_encoder.hpp
  include/rgbd/video_folder.hpp
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
  src/video_folder.cpp
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
set(RgbdLinkOptions "")
set(RgbdCompileOptions "")
set(RgbdCompileDefinitions
  CMAKE_RGBD_VERSION_MAJOR=${RGBD_VERSION_MAJOR}
  CMAKE_RGBD_VERSION_MINOR=${RGBD_VERSION_MINOR}
  CMAKE_RGBD_VERSION_PATCH=${RGBD_VERSION_PATCH}
  CMAKE_RGBD_VIDEOS_DIR="${PROJECT_SOURCE_DIR}/videos"
)

if(RGBD_OS_WINDOWS)
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
  if(NOT PROJECT_IS_TOP_LEVEL)
    # Set RGBD_FFMPEG_BIN in parent scope for parent project to copy the dll files.
    set(RGBD_FFMPEG_BIN ${FFMPEG_WIN64_DIR}/bin PARENT_SCOPE)
    set(RGBD_OPENCV_INCLUDE ${OPENCV_INCLUDE} PARENT_SCOPE)
    set(RGBD_OPENCV_LIB ${OPENCV_LIB} PARENT_SCOPE)
    set(RGBD_OPENCV_BIN ${OPENCV_BIN} PARENT_SCOPE)
  endif()
elseif(RGBD_OS_MAC)
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
  if(NOT PROJECT_IS_TOP_LEVEL)
    set(RGBD_OPENCV_INCLUDE ${OPENCV_INCLUDE} PARENT_SCOPE)
  endif()
  set(RGBD_OPENCV_LIB ${OPENCV_LIB} PARENT_SCOPE)
elseif(RGBD_OS_LINUX)
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_LINUX_DIR}/include
    ${LIBPNG_LINUX_DIR}/include
  )
  list(APPEND RgbdLibraryDependencies
    ${FFMPEG_BINARIES_DIR}/libvpx-binaries/1.10.0/x64-linux/lib/libvpx.a
    ${FFMPEG_BINARIES_DIR}/opus-binaries/e4d4b74/x64-linux/lib/libopus.a
    ${FFMPEG_LINUX_DIR}/lib/libavcodec.a
    ${FFMPEG_LINUX_DIR}/lib/libavformat.a
    ${FFMPEG_LINUX_DIR}/lib/libavutil.a
    ${LIBPNG_LINUX_DIR}/lib/libpng.a
  )
elseif(RGBD_OS_IOS)
  list(APPEND RgbdIncludeDirectories
    ${FFMPEG_IOS_DIR}/include
    ${LIBPNG_IOS_DIR}/include
  )
elseif(RGBD_OS_WASM)
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
  list(APPEND RgbdLinkOptions ${RgbdLibraryWasmOptions})
  list(APPEND RgbdCompileOptions ${RgbdLibraryWasmOptions})
endif()

add_library(rgbd-static STATIC ${RgbdSources})
target_include_directories(rgbd-static PRIVATE ${PROJECT_SOURCE_DIR}/include/rgbd)
target_include_directories(rgbd-static PUBLIC ${RgbdIncludeDirectories})
target_link_libraries(rgbd-static PUBLIC ${RgbdLibraryDependencies})
set_target_properties(rgbd-static PROPERTIES CXX_STANDARD 17)
set_target_properties(rgbd-static PROPERTIES OUTPUT_NAME rgbd)
target_link_options(rgbd-static PUBLIC ${RgbdLinkOptions})
target_compile_options(rgbd-static PUBLIC ${RgbdCompileOptions})
target_compile_definitions(rgbd-static PUBLIC ${RgbdCompileDefinitions})

if(RGBD_OS_MAC OR RGBD_OS_LINUX)
  # Building shared library requires more things linked.
  if(RGBD_OS_MAC)
    list(APPEND RgbdLibraryDependencies
      "-framework AudioToolbox"
      -lz
      -liconv
    )
  elseif(RGBD_OS_LINUX)
    # Position-independent code is needed to create a shared library.
    list(APPEND RgbdCompileOptions -fPIC)
    list(APPEND RgbdLinkOptions "-Wl,-Bsymbolic")
  endif()
  add_library(rgbd-shared SHARED ${RgbdSources})
  target_include_directories(rgbd-shared PRIVATE ${PROJECT_SOURCE_DIR}/include/rgbd)
  target_include_directories(rgbd-shared PUBLIC ${RgbdIncludeDirectories})
  target_link_libraries(rgbd-shared PUBLIC ${RgbdLibraryDependencies})
  set_target_properties(rgbd-shared PROPERTIES CXX_STANDARD 17)
  set_target_properties(rgbd-shared PROPERTIES OUTPUT_NAME rgbd-${RGBD_VERSION_MAJOR})
  target_link_options(rgbd-shared PUBLIC ${RgbdLinkOptions})
  target_compile_options(rgbd-shared PUBLIC ${RgbdCompileOptions})
  target_compile_definitions(rgbd-shared PUBLIC ${RgbdCompileDefinitions})
endif()

if(RGBD_OS_WINDOWS OR RGBD_OS_MAC OR RGBD_OS_LINUX)
  add_subdirectory(examples/depth_compression_example)
  add_subdirectory(src/cli)
endif()

if(RGBD_OS_MAC OR RGBD_OS_LINUX)
  install(TARGETS rgbd-cli DESTINATION bin)
  install(TARGETS rgbd-shared DESTINATION bin)
endif()
