/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "kinect_camera_calibration.hpp"

namespace rgbd
{
// kinect_transformation_unproject opened only for kinfu testing.
// TODO: hide this.
glm::vec3 kinect_transformation_unproject(const KinectCameraCalibration& calibration,
                                          const glm::vec2& point2d,
                                          bool& valid);
glm::vec3 compute_kinect_direction(const KinectCameraCalibration& calibration, const glm::vec2& uv);
} // namespace tg
