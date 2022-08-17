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

NativeInt16Array::NativeInt16Array(const vector<int16_t>& int16_values)
    : int16_values_{int16_values}
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
