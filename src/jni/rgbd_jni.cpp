#include "com_telegie_mainserver_LibrgbdJNI.h"

#include <rgbd/file_parser.hpp>

jint Java_com_telegie_mainserver_LibrgbdJNI_sayHello(JNIEnv *, jclass) {
    return 14141441;
}

jlong Java_com_telegie_mainserver_LibrgbdJNI_createFileParser(JNIEnv *env, jclass, jbyteArray byte_array) {
    size_t len = env->GetArrayLength(byte_array);
    unsigned char *buf = new unsigned char[len];
    env->GetByteArrayRegion(byte_array, 0, len, reinterpret_cast<jbyte *>(buf));

    auto file_parser{new rgbd::FileParser{buf, len}};
    return reinterpret_cast<jlong>(file_parser);
}

JNIEXPORT jbyteArray JNICALL
Java_com_telegie_mainserver_LibrgbdJNI_getCoverPngBytes(JNIEnv *env, jclass, jlong file_parser_ptr) {
    auto file_parser{reinterpret_cast<rgbd::FileParser *>(file_parser_ptr)};
    auto &cover_png_bytes{file_parser->info().cover_png_bytes()};

    jbyteArray array = env->NewByteArray(cover_png_bytes.size());
    env->SetByteArrayRegion(array, 0, cover_png_bytes.size(), reinterpret_cast<const jbyte *>(cover_png_bytes.data()));
    return array;
}