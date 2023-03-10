#include "math_utils.hpp"

#include <glm/gtx/quaternion.hpp>

namespace rgbd
{
glm::quat MathUtils::applyRotationRateAndGravityToRotation(
    const glm::quat& rotation,
    float delta_time_sec,
    const glm::vec3& rotation_rate,
    const glm::vec3& gravity
)
{
    glm::quat rotation_delta_from_rotation_rate{
        convertEulerAnglesToQuaternion(rotation_rate * delta_time_sec)};
    // Applying gyro to the previous rotation results in best-effort rotation.
    glm::quat rotation_with_correct_yaw{rotation * rotation_delta_from_rotation_rate};
    float yaw{extractYaw(rotation_with_correct_yaw)};

    glm::vec3 gravity_euler_angles{computeGravityCompensatingEulerAngles(gravity)};
    glm::vec3 euler_angles{gravity_euler_angles.x, yaw, gravity_euler_angles.z};

    glm::quat new_rotation{convertEulerAnglesToQuaternion(euler_angles)};
    return new_rotation;
}

// Below function convert_floor_to_gimbal_position_and_rotation is for
// finding a pair of position and rotation that satisfies the following conditions.
// R1. The rotation has a z-axis rotation, then an x-axis one, but no y-axis one.
//     The order is important to keep the center at on the x = 0 plane.
//     Notice that this order is different from the typical yaw-pitch-roll.
// R2. The rotation maps the up vector of Plane floor to [0, 1, 0]^T.
// P1. The position has y, but no x and z.
// P2. The position should adjust the height of the incoming stream properly,
//     so the floor of the incoming stream matches the viewer world's y = 0 plane.
//
// Starting with the rotation, let's notate the x-axis and z-axis rotation
// as R_x(theta) and R_z(psi)--theta and psi indicating the rotation angle in radians,
// and the up vector as u (=[x, y, z]^T).
//
// With these, we can write R2 as
// R_x(theta) * R_z(psi) * u = [0, 1, 0]^T, --- (1)
// where
//              | 1         0           0  |             | cos(psi) -sin(psi) 0 |
// R_x(theta) = | 0 cos(theta) -sin(theta) |, R_z(psi) = | sin(psi)  cos(psi) 0 |
//              | 0 sin(theta)  cos(theta) |             |       0         0  1 |.
// 
// (1) is equivalent to
// (R_z(psi))^-1 * (R_x(theta))^-1 * [0, 1, 0]^T = u. --- (2).
// 
// Given R_x(theta)^-1 = R_x(-theta), (2) is equivalent to
// R_z(-psi) * R_x(-theta) * [0, 1, 0]^T = u --- (3)
//
// Solving (3) gives
// x = cos(theta) * sin(psi), y = cos(theta) * cos(psi), z = -sin(theta). --- (4)
// A solution for (4), without the issue of moving behind the user, is
// theta = -arcsin(z), psi = atan2(x, y).
void MathUtils::convertGravityToThetaAndPsi(const glm::vec3& gravity, float& theta, float& psi)
{
    glm::vec3 gravity_direction{glm::normalize(gravity)};
    theta = glm::asin(gravity_direction.z);
    psi = 0.0f;
    if (glm::abs(gravity_direction.y) > 0.0001f)
        psi = glm::atan(-gravity_direction.x, -gravity_direction.y);
}

glm::vec3 MathUtils::computeGravityCompensatingEulerAngles(const glm::vec3& gravity)
{
    float theta{0.0f};
    float psi{0.0f};
    convertGravityToThetaAndPsi(gravity, theta, psi);
    return glm::vec3{theta, 0.0f, psi};
}

glm::quat MathUtils::computeGravityCompensatingRotation(const glm::vec3& gravity)
{
    auto euler_angles{computeGravityCompensatingEulerAngles(gravity)};
    return convertEulerAnglesToQuaternion(euler_angles);
}

glm::vec3 MathUtils::rotateVector3ByQuaternion(const glm::quat& quat, const glm::vec3& vec3)
{
    return glm::rotate(quat, vec3);
}

glm::quat MathUtils::convertEulerAnglesToQuaternion(const glm::vec3& euler_angles)
{
    glm::quat pitch_rotation{glm::angleAxis(euler_angles.x, glm::vec3(1.0f, 0.0f, 0.0f))};
    glm::quat yaw_rotation{glm::angleAxis(euler_angles.y, glm::vec3(0.0f, 1.0f, 0.0f))};
    glm::quat roll_rotation{glm::angleAxis(euler_angles.z, glm::vec3(0.0f, 0.0f, 1.0f))};

    return yaw_rotation * pitch_rotation * roll_rotation;
}

glm::quat MathUtils::multipleQuaternions(const glm::quat& quat1, const glm::quat& quat2)
{
    return quat1 * quat2;
}

float MathUtils::extractYaw(const glm::quat& quat)
{
    glm::vec3 front{glm::rotate(quat, glm::vec3{0.0f, 0.0f, -1.0f})};
    float yaw{atan2f(-front.x, -front.z)};
    return yaw;
}

float MathUtils::extractPitch(const glm::quat& quat)
{
    glm::vec3 front{glm::rotate(quat, glm::vec3{0.0f, 0.0f, -1.0f})};
    float xz_length{glm::sqrt(front.x * front.x + front.z * front.z)};
    float pitch{atan2f(front.y, xz_length)};
    return pitch;
}
}