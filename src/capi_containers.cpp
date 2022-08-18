#include "capi_containers.hpp"

namespace rgbd
{
NativeByteArray::NativeByteArray(const Bytes& byte_values)
    : byte_values_{byte_values}
{

}

NativeByteArray::NativeByteArray(Bytes&& byte_values)
    : byte_values_(std::move(byte_values))
{
}

NativeFloatArray::NativeFloatArray(vector<float>&& float_values)
    : float_values_(std::move(float_values))
{
}

NativeInt32Array::NativeInt32Array(const vector<int32_t>& int32_values)
    : int32_values_{int32_values}
{
}

NativeUInt8Array::NativeUInt8Array(const vector<uint8_t>& uint8_values)
    : uint8_values_{uint8_values}
{
}

NativeString::NativeString(const string& value)
    : value_{value}
{
}
}
