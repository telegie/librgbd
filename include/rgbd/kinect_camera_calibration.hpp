#pragma once

#include <rgbd/byte_utils.hpp>
#include "camera_calibration.hpp"

namespace rgbd
{
class KinectCameraCalibration : public CameraCalibration
{
public:
    KinectCameraCalibration(int color_width,
                            int color_height,
                            int depth_width,
                            int depth_height,
                            int resolution_width,
                            int resolution_height,
                            float cx,
                            float cy,
                            float fx,
                            float fy,
                            float k1,
                            float k2,
                            float k3,
                            float k4,
                            float k5,
                            float k6,
                            float codx,
                            float cody,
                            float p1,
                            float p2,
                            float max_radius_for_projection);
    unique_ptr<CameraCalibration> clone() const noexcept;
    static KinectCameraCalibration fromBytes(const Bytes& bytes, int& cursor);
    static KinectCameraCalibration fromJson(const json& json);
    Bytes toBytes() const noexcept;
    json toJson() const noexcept;
    CameraCalibrationType getCameraCalibrationType() const noexcept;
    int getColorWidth() const noexcept;
    int getColorHeight() const noexcept;
    int getDepthWidth() const noexcept;
    int getDepthHeight() const noexcept;
    glm::vec3 getDirection(const glm::vec2& uv) const noexcept;
    glm::vec2 getUv(const glm::vec3& direction) const noexcept;
    bool operator==(const KinectCameraCalibration& rhs) const
    {
        return color_width_ == rhs.color_width_ && color_height_ == rhs.color_height_ &&
               depth_width_ == rhs.depth_width_ && depth_height_ == rhs.depth_height_ &&
               resolution_width_ == rhs.resolution_width_ &&
               resolution_height_ == rhs.resolution_height_ && cx_ == rhs.cx_ && cy_ == rhs.cy_ &&
               fx_ == rhs.fx_ && fy_ == rhs.fy_ && k1_ == rhs.k1_ && k2_ == rhs.k2_ &&
               k3_ == rhs.k3_ && k4_ == rhs.k4_ && k5_ == rhs.k5_ && k6_ == rhs.k6_ &&
               codx_ == rhs.codx_ && cody_ == rhs.cody_ && p1_ == rhs.p1_ && p2_ == rhs.p2_ &&
               max_radius_for_projection_ == rhs.max_radius_for_projection_;
    }
    int resolution_width() const noexcept
    {
        return resolution_width_;
    }
    int resolution_height() const noexcept
    {
        return resolution_height_;
    }
    float cx() const noexcept
    {
        return cx_;
    }
    float cy() const noexcept
    {
        return cy_;
    }
    float fx() const noexcept
    {
        return fx_;
    }
    float fy() const noexcept
    {
        return fy_;
    }
    float k1() const noexcept
    {
        return k1_;
    }
    float k2() const noexcept
    {
        return k2_;
    }
    float k3() const noexcept
    {
        return k3_;
    }
    float k4() const noexcept
    {
        return k4_;
    }
    float k5() const noexcept
    {
        return k5_;
    }
    float k6() const noexcept
    {
        return k6_;
    }
    float codx() const noexcept
    {
        return codx_;
    }
    float cody() const noexcept
    {
        return cody_;
    }
    float p1() const noexcept
    {
        return p1_;
    }
    float p2() const noexcept
    {
        return p2_;
    }
    float max_radius_for_projection() const noexcept
    {
        return max_radius_for_projection_;
    }

private:
    int color_width_;
    int color_height_;
    int depth_width_;
    int depth_height_;

    // intrinsics
    int resolution_width_;
    int resolution_height_;
    float cx_;
    float cy_;
    float fx_;
    float fy_;
    float k1_;
    float k2_;
    float k3_;
    float k4_;
    float k5_;
    float k6_;
    float codx_;
    float cody_;
    float p1_;
    float p2_;
    float max_radius_for_projection_;
};
}
