#pragma once

#include <glm/glm.hpp>
#include "constants.hpp"

namespace rgbd
{
class MathUtils
{
public:
    static glm::quat applyRotationRateAndGravityToRotation(
        const glm::quat& rotation,
        float delta_time_sec,
        const glm::vec3& rotation_rate,
        const glm::vec3& gravity
    );
    static glm::quat convertEulerAnglesToQuaternion(const glm::vec3& euler_angles);
    static glm::vec3 convertQuaternionToEulerAngles(const glm::quat& quat);
    static glm::vec3 computeGravityCompensatingEulerAngles(const glm::vec3& gravity);
    static void convertRGBToYuv420(int width,
                                   int height,
                                   uint8_t* r_channel,
                                   uint8_t* g_channel,
                                   uint8_t* b_channel,
                                   uint8_t* y_channel,
                                   uint8_t* u_channel,
                                   uint8_t* v_channel);

private:
    static void convertGravityToThetaAndPsi(const glm::vec3& gravity, float& theta, float& psi);
};
}