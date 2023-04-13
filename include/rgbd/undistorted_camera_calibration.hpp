#pragma once

#include "byte_utils.hpp"
#include "camera_calibration.hpp"

namespace rgbd
{
class UndistortedCameraCalibration : public CameraCalibration
{
public:
    UndistortedCameraCalibration(int color_width,
                                 int color_height,
                                 int depth_width,
                                 int depth_height,
                                 float fx,
                                 float fy,
                                 float cx,
                                 float cy);
    unique_ptr<CameraCalibration> clone() const noexcept;
    static UndistortedCameraCalibration fromJson(const json& json);
    json toJson() const noexcept;
    CameraCalibrationType getCameraCalibrationType() const noexcept;
    int getColorWidth() const noexcept;
    int getColorHeight() const noexcept;
    int getDepthWidth() const noexcept;
    int getDepthHeight() const noexcept;
    glm::vec3 getDirection(const glm::vec2& uv) const noexcept;
    glm::vec2 getUv(const glm::vec3& direction) const noexcept;
    float fx() const noexcept
    {
        return fx_;
    }
    float fy() const noexcept
    {
        return fy_;
    }
    float cx() const noexcept
    {
        return cx_;
    }
    float cy() const noexcept
    {
        return cy_;
    }

private:
    int color_width_;
    int color_height_;
    int depth_width_;
    int depth_height_;

    // intrinsics
    float fx_;
    float fy_;
    float cx_;
    float cy_;
};
} // namespace rgbd
