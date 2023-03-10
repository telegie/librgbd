#pragma once

#include <glm/glm.hpp>

namespace rgbd
{
class MathUtils
{
public:
    static glm::vec3 computeGravityCompensatingEulerAngles(const glm::vec3& gravity);
    static glm::quat computeGravityCompensatingRotation(const glm::vec3& gravity);
    static glm::vec3 rotateVector3ByQuaternion(const glm::quat& quat, const glm::vec3& vec3);
    static glm::quat convertEulerAnglesToQuaternion(const glm::vec3& euler_angles);
    static glm::quat multipleQuaternions(const glm::quat& quat1, const glm::quat& quat2);
    static float extractYaw(const glm::quat& quat);
    static float extractPitch(const glm::quat& quat);
};
}