#include "byte_utils.hpp"

namespace rgbd
{
void append_bytes(Bytes& bytes1, const Bytes& bytes2)
{
    for (auto& byte : bytes2)
        bytes1.push_back(byte);
}
}
