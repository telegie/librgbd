add_executable(DepthCompressionExample
  depth_compression_example.cpp
)

# adding include directories due to the following issue:
# https://github.com/facebook/zstd/issues/2377
target_include_directories(DepthCompressionExample PUBLIC ${ZSTD_DIR}/lib)

# need to use zstd shared library for msvc
# reference: https://github.com/facebook/zstd/issues/1564
if(RGBD_PLATFORM_WINDOWS)
  target_link_libraries(DepthCompressionExample PUBLIC
    rgbd
    libzstd_shared
  )
elseif(RGBD_PLATFORM_MAC OR RGBD_PLATFORM_LINUX)
  target_link_libraries(DepthCompressionExample PUBLIC
    rgbd
    libzstd_static
  )
endif()

set_target_properties(DepthCompressionExample PROPERTIES
  CXX_STANDARD 17
  FOLDER "Examples"
)
