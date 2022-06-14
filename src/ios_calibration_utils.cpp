#include "ios_calibration_utils.hpp"

namespace tg
{
float get_magnification(float r, const vector<float>& lens_distortion_lookup_table, float r_max)
{
    if (r >= r_max)
        return lens_distortion_lookup_table.back();

    float val{r * static_cast<float>(lens_distortion_lookup_table.size() - 1) / r_max};
    int idx{static_cast<int>(val)};
    float frac{val - static_cast<float>(idx)};

    float mag1{lens_distortion_lookup_table[idx]};
    float mag2{lens_distortion_lookup_table[idx + 1]};

    return (1.0f - frac) * mag1 + frac * mag2;
}

float apply_magnification(float value, float center, float magnification)
{
    float diff{value - center};
    return diff + magnification * diff + center;
}

glm::vec3 compute_ios_direction(const IosCameraCalibration& calibration, const glm::vec2& uv)
{
    float fx{calibration.fx()};
    float fy{calibration.fy()};
    float ox{calibration.ox()};
    float oy{calibration.oy()};

    float reference_dimension_width{calibration.reference_dimension_width()};
    float reference_dimension_height{calibration.reference_dimension_height()};

    float lens_distortion_center_x{calibration.lens_distortion_center_x()};
    float lens_distortion_center_y{calibration.lens_distortion_center_y()};
    const vector<float>& lens_distortion_lookup_table{calibration.lens_distortion_lookup_table()};

    float delta_x_max{std::max(ox, reference_dimension_width - ox)};
    float delta_y_max{std::max(oy, reference_dimension_height - oy)};
    float r_max{std::sqrt(delta_x_max * delta_x_max + delta_y_max * delta_y_max)};

    // u, v are in the reference dimension
    float u{uv.x * reference_dimension_width};
    float v{(1.0f - uv.y) * reference_dimension_height};
    float r{std::sqrt(u * u + v * v)};

    float magnification{get_magnification(r, lens_distortion_lookup_table, r_max)};

    // uu, vv are with lens distortion calibrated
    float uu{apply_magnification(u, lens_distortion_center_x, magnification)};
    float vv{apply_magnification(v, lens_distortion_center_y, magnification)};

    return glm::vec3{(uu - ox) / fx, (vv - oy) / fy, -1.0f};
}
} // namespace tg
