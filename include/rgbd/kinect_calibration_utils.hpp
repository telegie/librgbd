/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "kinect_camera_calibration.hpp"

namespace rgbd
{
// kinect_transformation_unproject opened only for kinfu testing.
// TODO: hide this.

glm::vec2 kinect_transformation_project(const KinectCameraCalibration& calibration,
                                        const glm::vec3& point3d,
                                        bool& valid);
glm::vec3 kinect_transformation_unproject(const KinectCameraCalibration& calibration,
                                          const glm::vec2& point2d,
                                          bool& valid);
glm::vec3 compute_kinect_direction(const KinectCameraCalibration& calibration, const glm::vec2& uv);
glm::vec2 compute_kinect_uv(const KinectCameraCalibration& calibration, const glm::vec3& direction);
} // namespace rgbd
