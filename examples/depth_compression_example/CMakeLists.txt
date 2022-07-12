set(DepthCompressionExampleSources
  depth_compression_example.cpp
)

add_executable(DepthCompressionExample ${DepthCompressionExampleSources})
# need to use zstd shared library for msvc
# reference: https://github.com/facebook/zstd/issues/1564
if(RGBD_PLATFORM_WINDOWS)
  target_link_libraries(DepthCompressionExample PUBLIC
    rgbd
    libzstd_shared
  )
elseif(RGBD_PLATFORM_MAC)
  target_link_libraries(DepthCompressionExample PUBLIC
    rgbd
    libzstd_static
  )
endif()
set_target_properties(DepthCompressionExample PROPERTIES
  CXX_STANDARD 17
  FOLDER "Examples"
)
