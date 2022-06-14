#include "kinect_camera_calibration.hpp"

#include "kinect_calibration_utils.hpp"

namespace tg
{
KinectCameraCalibration::KinectCameraCalibration(int color_width,
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
                                                 float max_radius_for_projection)
    : color_width_{color_width}
    , color_height_{color_height}
    , depth_width_{depth_width}
    , depth_height_{depth_height}
    , resolution_width_{resolution_width}
    , resolution_height_{resolution_height}
    , cx_{cx}
    , cy_{cy}
    , fx_{fx}
    , fy_{fy}
    , k1_{k1}
    , k2_{k2}
    , k3_{k3}
    , k4_{k4}
    , k5_{k5}
    , k6_{k6}
    , codx_{codx}
    , cody_{cody}
    , p1_{p1}
    , p2_{p2}
    , max_radius_for_projection_{max_radius_for_projection}
{
}

KinectCameraCalibration KinectCameraCalibration::fromBytes(const Bytes& bytes, int& cursor)
{
    auto color_width{read_from_bytes<int>(bytes, cursor)};
    auto color_height{read_from_bytes<int>(bytes, cursor)};
    auto depth_width{read_from_bytes<int>(bytes, cursor)};
    auto depth_height{read_from_bytes<int>(bytes, cursor)};

    int resolution_width{read_from_bytes<int>(bytes, cursor)};
    int resolution_height{read_from_bytes<int>(bytes, cursor)};
    float cx{read_from_bytes<float>(bytes, cursor)};
    float cy{read_from_bytes<float>(bytes, cursor)};
    float fx{read_from_bytes<float>(bytes, cursor)};
    float fy{read_from_bytes<float>(bytes, cursor)};
    float k1{read_from_bytes<float>(bytes, cursor)};
    float k2{read_from_bytes<float>(bytes, cursor)};
    float k3{read_from_bytes<float>(bytes, cursor)};
    float k4{read_from_bytes<float>(bytes, cursor)};
    float k5{read_from_bytes<float>(bytes, cursor)};
    float k6{read_from_bytes<float>(bytes, cursor)};
    float codx{read_from_bytes<float>(bytes, cursor)};
    float cody{read_from_bytes<float>(bytes, cursor)};
    float p1{read_from_bytes<float>(bytes, cursor)};
    float p2{read_from_bytes<float>(bytes, cursor)};
    float max_radius_for_projection{read_from_bytes<float>(bytes, cursor)};
    return KinectCameraCalibration{color_width,
                                   color_height,
                                   depth_width,
                                   depth_height,
                                   resolution_width,
                                   resolution_height,
                                   cx,
                                   cy,
                                   fx,
                                   fy,
                                   k1,
                                   k2,
                                   k3,
                                   k4,
                                   k5,
                                   k6,
                                   codx,
                                   cody,
                                   p1,
                                   p2,
                                   max_radius_for_projection};
}

KinectCameraCalibration KinectCameraCalibration::fromJson(const json& json)
{
    int color_width{json["colorWidth"].get<int>()};
    int color_height{json["colorHeight"].get<int>()};
    int depth_width{json["depthWidth"].get<int>()};
    int depth_height{json["depthHeight"].get<int>()};

    int resolution_width{json["resolutionWidth"].get<int>()};
    int resolution_height{json["resolutionHeight"].get<int>()};
    float cx{json["cx"].get<float>()};
    float cy{json["cy"].get<float>()};
    float fx{json["fx"].get<float>()};
    float fy{json["fy"].get<float>()};
    float k1{json["k1"].get<float>()};
    float k2{json["k2"].get<float>()};
    float k3{json["k3"].get<float>()};
    float k4{json["k4"].get<float>()};
    float k5{json["k5"].get<float>()};
    float k6{json["k6"].get<float>()};
    float codx{json["codx"].get<float>()};
    float cody{json["cody"].get<float>()};
    float p1{json["p1"].get<float>()};
    float p2{json["p2"].get<float>()};
    float max_radius_for_projection{json["maxRadiusForProjection"].get<float>()};
    return KinectCameraCalibration{color_width,
                                   color_height,
                                   depth_width,
                                   depth_height,
                                   resolution_width,
                                   resolution_height,
                                   cx,
                                   cy,
                                   fx,
                                   fy,
                                   k1,
                                   k2,
                                   k3,
                                   k4,
                                   k5,
                                   k6,
                                   codx,
                                   cody,
                                   p1,
                                   p2,
                                   max_radius_for_projection};
}

Bytes KinectCameraCalibration::toBytes() const noexcept
{
    Bytes bytes;
    append_bytes(bytes, convert_to_bytes(color_width_));
    append_bytes(bytes, convert_to_bytes(color_height_));
    append_bytes(bytes, convert_to_bytes(depth_width_));
    append_bytes(bytes, convert_to_bytes(depth_height_));

    append_bytes(bytes, convert_to_bytes(resolution_width_));
    append_bytes(bytes, convert_to_bytes(resolution_height_));
    append_bytes(bytes, convert_to_bytes(cx_));
    append_bytes(bytes, convert_to_bytes(cy_));
    append_bytes(bytes, convert_to_bytes(fx_));
    append_bytes(bytes, convert_to_bytes(fy_));
    append_bytes(bytes, convert_to_bytes(k1_));
    append_bytes(bytes, convert_to_bytes(k2_));
    append_bytes(bytes, convert_to_bytes(k3_));
    append_bytes(bytes, convert_to_bytes(k4_));
    append_bytes(bytes, convert_to_bytes(k5_));
    append_bytes(bytes, convert_to_bytes(k6_));
    append_bytes(bytes, convert_to_bytes(codx_));
    append_bytes(bytes, convert_to_bytes(cody_));
    append_bytes(bytes, convert_to_bytes(p1_));
    append_bytes(bytes, convert_to_bytes(p2_));
    append_bytes(bytes, convert_to_bytes(max_radius_for_projection_));
    return bytes;
}

json KinectCameraCalibration::toJson() const noexcept
{
    return json{{"calibrationType", "azureKinect"},
                {"colorWidth", color_width_},
                {"colorHeight", color_height_},
                {"depthWidth", depth_width_},
                {"depthHeight", depth_height_},
                {"resolutionWidth", resolution_width_},
                {"resolutionHeight", resolution_height_},
                {"cx", cx_},
                {"cy", cy_},
                {"fx", fx_},
                {"fy", fy_},
                {"k1", k1_},
                {"k2", k2_},
                {"k3", k3_},
                {"k4", k4_},
                {"k5", k5_},
                {"k6", k6_},
                {"codx", codx_},
                {"cody", cody_},
                {"p1", p1_},
                {"p2", p2_},
                {"maxRadiusForProjection", max_radius_for_projection_}};
}

CameraDeviceType KinectCameraCalibration::getCameraDeviceType() const noexcept
{
    return CameraDeviceType::AzureKinect;
}

int KinectCameraCalibration::getColorWidth() const noexcept
{
    return color_width_;
}

int KinectCameraCalibration::getColorHeight() const noexcept
{
    return color_height_;
}

int KinectCameraCalibration::getDepthWidth() const noexcept
{
    return depth_width_;
}

int KinectCameraCalibration::getDepthHeight() const noexcept
{
    return depth_height_;
}

glm::vec3 KinectCameraCalibration::getDirection(const glm::vec2& uv) const noexcept
{
    return compute_kinect_direction(*this, uv);
}
} // namespace tg
