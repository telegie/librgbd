/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */
#pragma once

#include "camera_calibration.hpp"

namespace rgbd
{
class DirectionTable
{
public:
    DirectionTable(const rgbd::CameraCalibration& calibration);
    glm::vec3 getDirection(const glm::vec2& uv) const;
    int width() const noexcept
    {
        return width_;
    }
    int height() const noexcept
    {
        return height_;
    }
    const vector<glm::vec3>& directions() const noexcept
    {
        return directions_;
    }

private:
    int width_;
    int height_;
    vector<glm::vec3> directions_;
};
} // namespace rgbd
