#include "capi_utils.hpp"

namespace rgbd
{
CByteArray::CByteArray(const Bytes& byte_values)
    : byte_values_{byte_values}
{

}

CByteArray::CByteArray(Bytes&& byte_values)
    : byte_values_(std::move(byte_values))
{
}

CFloatArray::CFloatArray(vector<float>&& float_values)
    : float_values_(std::move(float_values))
{
}

CInt16Array::CInt16Array(const vector<int16_t>& int16_values)
    : int16_values_{int16_values}
{
}

CUInt8Array::CUInt8Array(const vector<uint8_t>& uint8_values)
    : uint8_values_{uint8_values}
{
}

CString::CString(const string& value)
    : value_{value}
{
}
}
