/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include <glm/glm.hpp>
#include "byte_utils.hpp"
#include "constants.hpp"

namespace rgbd
{
// Plane corresponding to ax + by + cz = d where normal = (a, b, c) and constant = d.
class Plane
{
public:
    Plane(const glm::vec3& normal, const float constant) noexcept;
    Plane() noexcept;
    static Plane fromBytes(const Bytes& bytes);
    Bytes toBytes() const noexcept;
    const glm::vec3& normal() const
    {
        return normal_;
    }
    glm::vec3& normal()
    {
        return normal_;
    }
    void set_normal(const glm::vec3& normal)
    {
        normal_ = normal;
    }
    float constant() const
    {
        return constant_;
    }
    void set_constant(float constant)
    {
        constant_ = constant;
    }

private:
    glm::vec3 normal_;
    float constant_;
};
} // namespace tg
