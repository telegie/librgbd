/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "plane.hpp"

namespace rgbd
{
Plane::Plane(const glm::vec3& normal, const float constant) noexcept
    : normal_{normal}
    , constant_{constant}
{
}

Plane::Plane() noexcept
    : normal_{}
    , constant_{0.0f}
{
}

Plane Plane::fromBytes(const Bytes& bytes)
{
    int cursor{0};
    float normal_x{read_from_bytes<float>(bytes, cursor)};
    float normal_y{read_from_bytes<float>(bytes, cursor)};
    float normal_z{read_from_bytes<float>(bytes, cursor)};
    float constant{read_from_bytes<float>(bytes, cursor)};
    return Plane{glm::vec3{normal_x, normal_y, normal_z}, constant};
}
Bytes Plane::toBytes() const noexcept
{
    Bytes bytes;
    append_bytes(bytes, convert_to_bytes(normal_.x));
    append_bytes(bytes, convert_to_bytes(normal_.y));
    append_bytes(bytes, convert_to_bytes(normal_.z));
    append_bytes(bytes, convert_to_bytes(constant_));
    return bytes;
}
} // namespace rgbd
