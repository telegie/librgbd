#include "rgbd_jni.h"

#include <rgbd/file_parser.hpp>

jstring Java_com_telegie_mainserver_LibrgbdJNI_nativeGetVersion(JNIEnv *env, jclass) {
    std::string version{fmt::format("{}.{}.{}", rgbd::MAJOR_VERSION, rgbd::MINOR_VERSION, rgbd::PATCH_VERSION)};
    jstring result{env->NewStringUTF(version.c_str())};
    return result;
}

jlong Java_com_telegie_mainserver_LibrgbdJNI_nativeCreateFileParser(JNIEnv *env, jclass, jbyteArray byte_array)
{
    size_t len = env->GetArrayLength(byte_array);
    unsigned char *buf = new unsigned char[len];
    env->GetByteArrayRegion(byte_array, 0, len, reinterpret_cast<jbyte *>(buf));

    auto file_parser{new rgbd::FileParser{buf, len}};
    return reinterpret_cast<jlong>(file_parser);
}

void Java_com_telegie_mainserver_LibrgbdJNI_nativeDeleteFileParser(JNIEnv *, jclass, jlong file_parser_ptr)
{
    auto file_parser{reinterpret_cast<rgbd::FileParser *>(file_parser_ptr)};
    delete file_parser;
}


jdouble Java_com_telegie_mainserver_LibrgbdJNI_nativeGetDurationUs(JNIEnv *, jclass, jlong file_parser_ptr)
{
    auto file_parser{reinterpret_cast<rgbd::FileParser *>(file_parser_ptr)};
    auto duration_us{file_parser->info().duration_us()};
    return duration_us;
}

jbyteArray
Java_com_telegie_mainserver_LibrgbdJNI_nativeGetCoverPngBytes(JNIEnv *env, jclass, jlong file_parser_ptr)
{
    auto file_parser{reinterpret_cast<rgbd::FileParser *>(file_parser_ptr)};
    auto &cover_png_bytes{file_parser->info().cover_png_bytes()};

    jbyteArray array = env->NewByteArray(cover_png_bytes.size());
    env->SetByteArrayRegion(array, 0, cover_png_bytes.size(), reinterpret_cast<const jbyte *>(cover_png_bytes.data()));
    return array;
}