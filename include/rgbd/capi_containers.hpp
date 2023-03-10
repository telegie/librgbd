#pragma once

#include "constants.hpp"
#include <glm/glm.hpp>

namespace rgbd
{
// These native classes are for making it clear the process of
// other runtimes using C APIs and holding memory in heap. 
using NativeByteArray = Bytes;
using NativeFloatArray = vector<float>;
using NativeInt32Array = vector<int32_t>;
using NativeQuaternion = glm::quat;
using NativeUInt8Array = vector<uint8_t>;
using NativeString = string;
using NativeVector3 = glm::vec3;
}
