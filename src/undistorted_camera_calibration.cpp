#include "undistorted_camera_calibration.hpp"

namespace rgbd
{
UndistortedCameraCalibration::UndistortedCameraCalibration(
    int color_width,
    int color_height,
    int depth_width,
    int depth_height,
    float fx,
    float fy,
    float cx,
    float cy)
    : color_width_{color_width}
    , color_height_{color_height}
    , depth_width_{depth_width}
    , depth_height_{depth_height}
    , fx_{fx}
    , fy_{fy}
    , cx_{cx}
    , cy_{cy}
{
}

UndistortedCameraCalibration UndistortedCameraCalibration::fromJson(const json& json)
{
    int color_width{json["colorWidth"].get<int>()};
    int color_height{json["colorHeight"].get<int>()};
    int depth_width{json["depthWidth"].get<int>()};
    int depth_height{json["depthHeight"].get<int>()};

    float fx{json["fx"].get<float>()};
    float fy{json["fy"].get<float>()};
    float cx{json["cx"].get<float>()};
    float cy{json["cy"].get<float>()};

    return UndistortedCameraCalibration{
        color_width, color_height, depth_width, depth_height, fx, fy, cx, cy};
}

json UndistortedCameraCalibration::toJson() const noexcept
{
    return json{{"calibrationType", "undistorted"},
                {"colorWidth", color_width_},
                {"colorHeight", color_height_},
                {"depthWidth", depth_width_},
                {"depthHeight", depth_height_},
                {"fx", fx_},
                {"fy", fy_},
                {"cx", cx_},
                {"cy", cy_}};
}

CameraDeviceType UndistortedCameraCalibration::getCameraDeviceType() const noexcept
{
    return CameraDeviceType::Undistorted;
}

int UndistortedCameraCalibration::getColorWidth() const noexcept
{
    return color_width_;
}

int UndistortedCameraCalibration::getColorHeight() const noexcept
{
    return color_height_;
}

int UndistortedCameraCalibration::getDepthWidth() const noexcept
{
    return depth_width_;
}

int UndistortedCameraCalibration::getDepthHeight() const noexcept
{
    return depth_height_;
}

glm::vec3 UndistortedCameraCalibration::getDirection(const glm::vec2& uv) const noexcept
{
    return glm::vec3{(uv.x - cx_) / fx_, (uv.y - cy_) / fy_, -1.0f};
}

glm::vec2 UndistortedCameraCalibration::getUv(const glm::vec3& direction) const noexcept
{
    float x{direction.x / -direction.z};
    float y{direction.y / -direction.z};
    // float z{-1.0f}

    return glm::vec2{fx_ * x + cx_, fy_ * y + cy_};
}
} // namespace rgbd
