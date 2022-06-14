/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "kinect_camera_calibration.hpp"

namespace tg
{
glm::vec3 compute_kinect_direction(const KinectCameraCalibration& calibration, const glm::vec2& uv);
} // namespace tg
