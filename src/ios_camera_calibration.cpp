#include "ios_camera_calibration.hpp"

#include "ios_calibration_utils.hpp"

namespace rgbd
{
IosCameraCalibration::IosCameraCalibration(int color_width,
                                           int color_height,
                                           int depth_width,
                                           int depth_height,
                                           float fx,
                                           float fy,
                                           float ox,
                                           float oy,
                                           float reference_dimension_width,
                                           float reference_dimension_height,
                                           float lens_distortion_center_x,
                                           float lens_distortion_center_y,
                                           gsl::span<const float> lens_distortion_lookup_table)
    : color_width_{color_width}
    , color_height_{color_height}
    , depth_width_{depth_width}
    , depth_height_{depth_height}
    , fx_{fx}
    , fy_{fy}
    , ox_{ox}
    , oy_{oy}
    , reference_dimension_width_{reference_dimension_width}
    , reference_dimension_height_{reference_dimension_height}
    , lens_distortion_center_x_{lens_distortion_center_x}
    , lens_distortion_center_y_{lens_distortion_center_y}
    , lens_distortion_lookup_table_{}
{
    lens_distortion_lookup_table_.assign(lens_distortion_lookup_table.begin(), lens_distortion_lookup_table.end());
}

IosCameraCalibration IosCameraCalibration::fromBytes(const Bytes& bytes, int& cursor)
{
    int color_width{read_from_bytes<int>(bytes, cursor)};
    int color_height{read_from_bytes<int>(bytes, cursor)};
    int depth_width{read_from_bytes<int>(bytes, cursor)};
    int depth_height{read_from_bytes<int>(bytes, cursor)};

    float fx{read_from_bytes<float>(bytes, cursor)};
    float fy{read_from_bytes<float>(bytes, cursor)};
    float ox{read_from_bytes<float>(bytes, cursor)};
    float oy{read_from_bytes<float>(bytes, cursor)};
    float reference_dimension_width{read_from_bytes<float>(bytes, cursor)};
    float reference_dimension_height{read_from_bytes<float>(bytes, cursor)};
    float lens_distortion_center_x{read_from_bytes<float>(bytes, cursor)};
    float lens_distortion_center_y{read_from_bytes<float>(bytes, cursor)};

    int lens_distortion_lookup_table_size{read_from_bytes<int>(bytes, cursor)};
    vector<float> lens_distortion_lookup_table(lens_distortion_lookup_table_size);
    for (gsl::index i{0}; i < lens_distortion_lookup_table_size; ++i)
        lens_distortion_lookup_table[i] = read_from_bytes<float>(bytes, cursor);

    return IosCameraCalibration{color_width,
                                color_height,
                                depth_width,
                                depth_height,
                                fx,
                                fy,
                                ox,
                                oy,
                                reference_dimension_width,
                                reference_dimension_height,
                                lens_distortion_center_x,
                                lens_distortion_center_y,
                                lens_distortion_lookup_table};
}

IosCameraCalibration IosCameraCalibration::fromJson(const json& json)
{
    int color_width{json["colorWidth"].get<int>()};
    int color_height{json["colorHeight"].get<int>()};
    int depth_width{json["depthWidth"].get<int>()};
    int depth_height{json["depthHeight"].get<int>()};

    float fx{json["fx"].get<float>()};
    float fy{json["fy"].get<float>()};
    float ox{json["ox"].get<float>()};
    float oy{json["oy"].get<float>()};
    float reference_dimension_width{json["referenceDimensionWidth"].get<float>()};
    float reference_dimension_height{json["referenceDimensionHeight"].get<float>()};
    float lens_distortion_center_x{json["lensDistortionCenterX"].get<float>()};
    float lens_distortion_center_y{json["lensDistortionCenterY"].get<float>()};
    vector<float> lens_distortion_lookup_table;
    for (auto& value : json["lensDistortionLookupTable"])
        lens_distortion_lookup_table.push_back(value.get<float>());

    return IosCameraCalibration{color_width,
                                color_height,
                                depth_width,
                                depth_height,
                                fx,
                                fy,
                                ox,
                                oy,           reference_dimension_width,
                                reference_dimension_height,
                                lens_distortion_center_x,
                                lens_distortion_center_y,
                                lens_distortion_lookup_table};
}

Bytes IosCameraCalibration::toBytes() const noexcept
{
    Bytes bytes;
    append_bytes(bytes, convert_to_bytes(color_width_));
    append_bytes(bytes, convert_to_bytes(color_height_));
    append_bytes(bytes, convert_to_bytes(depth_width_));
    append_bytes(bytes, convert_to_bytes(depth_height_));

    append_bytes(bytes, convert_to_bytes(fx_));
    append_bytes(bytes, convert_to_bytes(fy_));
    append_bytes(bytes, convert_to_bytes(ox_));
    append_bytes(bytes, convert_to_bytes(oy_));
    append_bytes(bytes, convert_to_bytes(reference_dimension_width_));
    append_bytes(bytes, convert_to_bytes(reference_dimension_height_));
    append_bytes(bytes, convert_to_bytes(lens_distortion_center_x_));
    append_bytes(bytes, convert_to_bytes(lens_distortion_center_y_));

    append_bytes(bytes, convert_to_bytes(gsl::narrow<int>(lens_distortion_lookup_table_.size())));
    for (float value : lens_distortion_lookup_table_)
        append_bytes(bytes, convert_to_bytes(value));

    return bytes;
}

json IosCameraCalibration::toJson() const noexcept
{
    return json{{"calibrationType", "ios"},
                {"colorWidth", color_width_},
                {"colorHeight", color_height_},
                {"depthWidth", depth_width_},
                {"depthHeight", depth_height_},
                {"fx", fx_},
                {"fy", fy_},
                {"ox", ox_},
                {"oy", oy_},
                {"referenceDimensionWidth", reference_dimension_width_},
                {"referenceDimensionHeight", reference_dimension_height_},
                {"lensDistortionCenterX", lens_distortion_center_x_},
                {"lensDistortionCenterY", lens_distortion_center_y_},
                {"lensDistortionLookupTable", lens_distortion_lookup_table_}};
}

CameraDeviceType IosCameraCalibration::getCameraDeviceType() const noexcept
{
    return CameraDeviceType::IOS;
}

int IosCameraCalibration::getColorWidth() const noexcept
{
    return color_width_;
}

int IosCameraCalibration::getColorHeight() const noexcept
{
    return color_height_;
}

int IosCameraCalibration::getDepthWidth() const noexcept
{
    return depth_width_;
}

int IosCameraCalibration::getDepthHeight() const noexcept
{
    return depth_height_;
}

glm::vec3 IosCameraCalibration::getDirection(const glm::vec2& uv) const noexcept
{
    return compute_ios_direction(*this, uv);
}
} // namespace tg
