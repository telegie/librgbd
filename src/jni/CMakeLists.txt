add_library(rgbd-jni SHARED
  com_telegie_mainserver_LibrgbdJNI.h
  rgbd_jni.cpp
)

target_include_directories(rgbd-jni PRIVATE ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
target_link_libraries(rgbd-jni PRIVATE rgbd)
set_target_properties(rgbd-jni PROPERTIES CXX_STANDARD 17)
set_target_properties(rgbd-jni PROPERTIES OUTPUT_NAME rgbd-jni-${RGBD_VERSION_MAJOR})
