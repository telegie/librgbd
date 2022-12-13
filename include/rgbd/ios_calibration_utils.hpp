#pragma once

#include "ios_camera_calibration.hpp"

namespace rgbd
{
glm::vec3 compute_ios_direction(const IosCameraCalibration& calibration, const glm::vec2& uv);
glm::vec2 compute_ios_uv(const IosCameraCalibration& calibration, const glm::vec3& direction);
} // namespace rgbd
