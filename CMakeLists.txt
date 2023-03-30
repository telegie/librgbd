cmake_minimum_required(VERSION 3.18)
project(librgbd)

set(RGBD_VERSION_MAJOR 0)
set(RGBD_VERSION_MINOR 1)
set(RGBD_VERSION_PATCH 0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)

if(MSVC)
  add_compile_options(/MP)
endif()

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

set(RGBD_ARCH_ARM64 FALSE)
set(RGBD_ARCH_X64 FALSE)
if(RGBD_OS_MAC)
  # Check for CMAKE_OSX_ARCHITECTURES set by users,
  # if not, use CMAKE_SYSTEM_PROCESSOR inferred by CMake.
  if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
    set(RGBD_ARCH_ARM64 TRUE)
  elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
    set(RGBD_ARCH_X64 TRUE)
  elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(RGBD_ARCH_ARM64 TRUE)
  elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(RGBD_ARCH_X64 TRUE)
  endif()
endif()
message("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")

set(RGBD_WEB_DIR ${PROJECT_SOURCE_DIR}/web)
if(NOT PROJECT_IS_TOP_LEVEL)
  set(RGBD_WEB_DIR ${RGBD_WEB_DIR} PARENT_SCOPE)
endif()

### CATCH2 BEGIN
add_subdirectory(deps/Catch2)
### CATCH2 END

### CLI BEGIN ###
add_subdirectory(deps/cli EXCLUDE_FROM_ALL)
### CLI END ###

### CXXOPTS BEGIN ###
add_subdirectory(deps/cxxopts EXCLUDE_FROM_ALL)
### CXXOPTS END ###

### FFMPEG BEGIN ###
set(FFMPEG_BINARIES_DIR ${PROJECT_SOURCE_DIR}/deps/ffmpeg-binaries)
set(FFMPEG_WIN64_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/x64-windows)
if(RGBD_ARCH_ARM64)
  set(FFMPEG_MAC_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/arm64-mac)
elseif(RGBD_ARCH_X64)
  set(FFMPEG_MAC_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/x64-mac)
endif()
set(FFMPEG_LINUX_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/x64-linux)
set(FFMPEG_IOS_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/arm64-ios)
set(FFMPEG_WASM_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/wasm32-emscripten)
# Using the below multi-threaded version speeds up VP8 decoding a lot but
# in iOS, causes maximum class stack size exceededing errors.
# In Mac Safari, the speed up only lasts the first few seconds. 
# set(FFMPEG_WASM_DIR ${FFMPEG_BINARIES_DIR}/4.4.1/wasm32-emscripten-mt)
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
set(LIBPNG_BUILD_DIR ${PROJECT_SOURCE_DIR}/deps/libpng-build)
set(LIBPNG_WIN64_DIR ${LIBPNG_BUILD_DIR}/output/x64-windows)
if(RGBD_ARCH_ARM64)
  set(LIBPNG_MAC_DIR ${LIBPNG_BUILD_DIR}/output/arm64-mac)
elseif(RGBD_ARCH_X64)
  set(LIBPNG_MAC_DIR ${LIBPNG_BUILD_DIR}/output/x64-mac)
endif()
set(LIBPNG_LINUX_DIR ${LIBPNG_BUILD_DIR}/output/x64-linux)
set(LIBPNG_IOS_DIR ${LIBPNG_BUILD_DIR}/output/arm64-ios)

# libpng requires zlib
set(ZLIB_BUILD_DIR ${LIBPNG_BUILD_DIR}/zlib-binaries)
set(ZLIB_WIN64_DIR ${ZLIB_BUILD_DIR}/output/x64-windows)
set(ZLIB_LINUX_DIR ${ZLIB_BUILD_DIR}/output/x64-linux)
### LIBPNG END ###

### LIBVPX BEGIN ###
set(LIBVPX_BINARIES_DIR ${FFMPEG_BINARIES_DIR}/libvpx-binaries)
if(RGBD_ARCH_ARM64)
  set(LIBVPX_MAC_DIR ${LIBVPX_BINARIES_DIR}/1.12.0/arm64-mac)
elseif(RGBD_ARCH_X64)
  set(LIBVPX_MAC_DIR ${LIBVPX_BINARIES_DIR}/1.12.0/x64-mac)
endif()
set(LIBVPX_LINUX_DIR ${LIBVPX_BINARIES_DIR}/1.12.0/x64-linux)
set(LIBVPX_WASM_DIR ${LIBVPX_BINARIES_DIR}/1.12.0/wasm32-emscripten)
### LIBVPX END ###

### OPUS BEGIN ###
set(OPUS_BINARIES_DIR ${FFMPEG_BINARIES_DIR}/opus-binaries)
if(RGBD_ARCH_ARM64)
  set(OPUS_MAC_DIR ${OPUS_BINARIES_DIR}/e4d4b74/arm64-mac)
elseif(RGBD_ARCH_X64)
  set(OPUS_MAC_DIR ${OPUS_BINARIES_DIR}/e4d4b74/x64-mac)
endif()
set(OPUS_LINUX_DIR ${OPUS_BINARIES_DIR}/e4d4b74/x64-linux)
set(OPUS_WASM_DIR ${OPUS_BINARIES_DIR}/e4d4b74/wasm32-emscripten)
### OPUS END ###

### PYBIND11 BEGIN ###
add_subdirectory(deps/pybind11)
### PYBIND11 END ###

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
  set(ZSTD_BUILD_TESTS OFF)
  add_subdirectory(deps/zstd/build/cmake EXCLUDE_FROM_ALL)
  set_target_properties(libzstd_shared PROPERTIES FOLDER "Dependencies")
endif()
### ZSTD END ###
set(RGBD_SOURCES
  include/rgbd/audio_decoder.hpp
  include/rgbd/audio_encoder.hpp
  include/rgbd/audio_frame.hpp
  include/rgbd/byte_utils.hpp
  include/rgbd/camera_calibration.hpp
  include/rgbd/capi_containers.hpp
  include/rgbd/color_decoder.hpp
  include/rgbd/color_encoder.hpp
  include/rgbd/constants.hpp
  include/rgbd/depth_decoder.hpp
  include/rgbd/depth_encoder.hpp
  include/rgbd/direction_table.hpp
  include/rgbd/ffmpeg_utils.hpp
  include/rgbd/file.hpp
  include/rgbd/file_bytes_builder.hpp
  include/rgbd/file_parser.hpp
  include/rgbd/file_writer.hpp
  include/rgbd/frame_mapper.hpp
  include/rgbd/integer_frame.hpp
  include/rgbd/ios_calibration_utils.hpp
  include/rgbd/ios_camera_calibration.hpp
  include/rgbd/kinect_calibration_utils.hpp
  include/rgbd/kinect_camera_calibration.hpp
  include/rgbd/math_utils.hpp
  include/rgbd/plane.hpp
  include/rgbd/png_utils.hpp
  include/rgbd/rgbd.hpp
  include/rgbd/rgbd_capi.h
  include/rgbd/rvl.hpp
  include/rgbd/rvl_decoder.hpp
  include/rgbd/rvl_encoder.hpp
  include/rgbd/tdc1_decoder.hpp
  include/rgbd/tdc1_encoder.hpp
  include/rgbd/undistorted_camera_calibration.hpp
  include/rgbd/video_folder.hpp
  include/rgbd/video_frame.hpp
  include/rgbd/yuv_frame.hpp
  src/audio_encoder.cpp
  src/audio_decoder.cpp
  src/audio_frame.cpp
  src/byte_utils.cpp
  src/camera_calibration.cpp
  src/capi_containers.cpp
  src/color_decoder.cpp
  src/color_encoder.cpp
  src/constants.cpp
  src/depth_decoder.cpp
  src/depth_encoder.cpp
  src/direction_table.cpp
  src/ffmpeg_utils.cpp
  src/file.cpp
  src/file_bytes_builder.cpp
  src/file_parser.cpp
  src/file_writer.cpp
  src/frame_mapper.cpp
  src/integer_frame.cpp
  src/ios_calibration_utils.cpp
  src/ios_camera_calibration.cpp
  src/kinect_calibration_utils.cpp
  src/kinect_camera_calibration.cpp
  src/math_utils.cpp
  src/plane.cpp
  src/png_utils.cpp
  src/rgbd_capi.cpp
  src/rvl.cpp
  src/rvl_decoder.cpp
  src/rvl_encoder.cpp
  src/tdc1_decoder.cpp
  src/tdc1_encoder.cpp
  src/undistorted_camera_calibration.cpp
  src/video_folder.cpp
  src/video_frame.cpp
  src/yuv_frame.cpp
)
set(RGBD_INCLUDES
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/deps/libebml
  ${PROJECT_BINARY_DIR}/deps/libmatroska
  ${GLM_DIR}
  ${GSL_INCLUDE_DIR}
)
set(RGBD_DEPENDENCIES
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
  list(APPEND RGBD_INCLUDES
    ${FFMPEG_WIN64_DIR}/include
    ${LIBPNG_WIN64_DIR}/include
  )
  list(APPEND RGBD_DEPENDENCIES
    ${FFMPEG_WIN64_DIR}/bin/avcodec.lib
    ${FFMPEG_WIN64_DIR}/bin/avformat.lib
    ${FFMPEG_WIN64_DIR}/bin/avutil.lib
    ${LIBPNG_WIN64_DIR}/lib/libpng16_static.lib
    ${ZLIB_WIN64_DIR}/lib/zlibstatic.lib
  )
  list(APPEND RgbdCompileDefinitions CMAKE_RGBD_OS_WINDOWS)
  if(NOT PROJECT_IS_TOP_LEVEL)
    # Set RGBD_FFMPEG_BIN in parent scope for parent project to copy the dll files.
    set(RGBD_FFMPEG_BIN ${FFMPEG_WIN64_DIR}/bin PARENT_SCOPE)
  endif()
elseif(RGBD_OS_MAC)
  list(APPEND RGBD_INCLUDES
    ${FFMPEG_MAC_DIR}/include
    ${LIBPNG_MAC_DIR}/include
  )
  list(APPEND RGBD_DEPENDENCIES
    ${FFMPEG_MAC_DIR}/lib/libavcodec.a
    ${FFMPEG_MAC_DIR}/lib/libavformat.a
    ${FFMPEG_MAC_DIR}/lib/libavutil.a
    ${LIBVPX_MAC_DIR}/lib/libvpx.a
    ${OPUS_MAC_DIR}/lib/libopus.a
    ${LIBPNG_MAC_DIR}/lib/libpng.a
    "-framework AudioToolbox"
    -lz
    -liconv
  )
  list(APPEND RgbdLinkOptions "-Wl,-no_compact_unwind")
  list(APPEND RgbdCompileDefinitions CMAKE_RGBD_OS_MAC)
elseif(RGBD_OS_LINUX)
  list(APPEND RGBD_INCLUDES
    ${FFMPEG_LINUX_DIR}/include
    ${LIBPNG_LINUX_DIR}/include
  )
  # The order here matters. libvpx and libopus should be listed
  # after the ffmpeg ones.
  list(APPEND RGBD_DEPENDENCIES
    ${FFMPEG_LINUX_DIR}/lib/libavcodec.a
    ${FFMPEG_LINUX_DIR}/lib/libavformat.a
    ${FFMPEG_LINUX_DIR}/lib/libavutil.a
    ${LIBVPX_LINUX_DIR}/lib/libvpx.a
    ${OPUS_LINUX_DIR}/lib/libopus.a
    ${LIBPNG_LINUX_DIR}/lib/libpng.a
    ${ZLIB_LINUX_DIR}/lib/libz.a 
    -llzma
  )
  list(APPEND RgbdCompileDefinitions CMAKE_RGBD_OS_LINUX)
elseif(RGBD_OS_IOS)
  list(APPEND RGBD_INCLUDES
    ${FFMPEG_IOS_DIR}/include
    ${LIBPNG_IOS_DIR}/include
  )
  list(APPEND RgbdCompileDefinitions CMAKE_RGBD_OS_IOS)
elseif(RGBD_OS_WASM)
  list(APPEND RGBD_INCLUDES
    ${FFMPEG_WASM_DIR}/include
  )
  list(APPEND RGBD_DEPENDENCIES
    ${FFMPEG_WASM_DIR}/lib/libavcodec.a
    ${FFMPEG_WASM_DIR}/lib/libavformat.a
    ${FFMPEG_WASM_DIR}/lib/libavutil.a
    ${FFMPEG_WASM_DIR}/lib/libswresample.a
    ${LIBVPX_WASM_DIR}/lib/libvpx.a
    ${OPUS_WASM_DIR}/lib/libopus.a
  )
  list(APPEND RgbdCompileDefinitions CMAKE_RGBD_OS_WASM)

  set(RgbdLibraryWasmOptions
    "SHELL:-s USE_LIBPNG=1"
  )
  set(RgbdLibraryWasmLinkOptions
    "-Wl,--whole-archive"
  )
  list(APPEND RgbdLinkOptions ${RgbdLibraryWasmOptions} ${RgbdLibraryWasmLinkOptions})
  list(APPEND RgbdCompileOptions ${RgbdLibraryWasmOptions})
endif()

add_library(rgbd-static STATIC ${RGBD_SOURCES})
target_include_directories(rgbd-static PRIVATE ${PROJECT_SOURCE_DIR}/include/rgbd)
target_include_directories(rgbd-static PUBLIC ${RGBD_INCLUDES})
target_link_libraries(rgbd-static PUBLIC ${RGBD_DEPENDENCIES})
set_target_properties(rgbd-static PROPERTIES CXX_STANDARD 17)
target_link_options(rgbd-static PUBLIC ${RgbdLinkOptions})
target_compile_options(rgbd-static PUBLIC ${RgbdCompileOptions})
target_compile_definitions(rgbd-static PUBLIC ${RgbdCompileDefinitions})

if(RGBD_OS_WINDOWS OR RGBD_OS_MAC OR RGBD_OS_LINUX)
  if(RGBD_OS_LINUX)
    # Position-independent code is needed to create a shared library.
    list(APPEND RgbdCompileOptions -fPIC)
    list(APPEND RgbdLinkOptions "-Wl,-Bsymbolic")
  endif()
  add_library(rgbd-shared SHARED ${RGBD_SOURCES})
  target_include_directories(rgbd-shared PRIVATE ${PROJECT_SOURCE_DIR}/include/rgbd)
  target_include_directories(rgbd-shared PUBLIC ${RGBD_INCLUDES})
  target_link_libraries(rgbd-shared PUBLIC ${RGBD_DEPENDENCIES})
  set_target_properties(rgbd-shared PROPERTIES CXX_STANDARD 17)
  set_target_properties(rgbd-shared PROPERTIES OUTPUT_NAME rgbd)
  target_link_options(rgbd-shared PUBLIC ${RgbdLinkOptions})
  target_compile_options(rgbd-shared PUBLIC ${RgbdCompileOptions})
  target_compile_definitions(rgbd-shared PUBLIC ${RgbdCompileDefinitions})
endif()

if(RGBD_OS_WINDOWS OR RGBD_OS_MAC OR RGBD_OS_LINUX)
  add_subdirectory(examples/depth_compression_example)
  add_subdirectory(examples/minimum_example)
  add_subdirectory(src/cli)
  add_subdirectory(src/python)
endif()

if(RGBD_OS_WINDOWS OR RGBD_OS_MAC OR RGBD_OS_LINUX)
  install(TARGETS rgbd-shared DESTINATION bin)
  install(TARGETS rgbd-static DESTINATION lib)
  install(TARGETS rgbd-cli DESTINATION bin)
  install(FILES ${CMAKE_SOURCE_DIR}/include/rgbd/rgbd_capi.h
          DESTINATION include/rgbd)
endif()

if(RGBD_OS_WINDOWS OR RGBD_OS_MAC OR RGBD_OS_LINUX)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
