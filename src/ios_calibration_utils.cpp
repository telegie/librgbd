#include "ios_calibration_utils.hpp"

namespace rgbd
{
float get_magnification(float r, const vector<float>& lookup_table, float r_max)
{
    if (lookup_table.size() == 0)
        return 0.0f;

    if (r >= r_max)
        return lookup_table.back();

    float val{r * static_cast<float>(lookup_table.size() - 1) / r_max};
    int idx{static_cast<int>(val)};
    float frac{val - static_cast<float>(idx)};

    float mag1{lookup_table[idx]};
    float mag2{lookup_table[idx + 1]};

    return (1.0f - frac) * mag1 + frac * mag2;
}

// Reference: https://github.com/shu223/iOS-Depth-Sampler/issues/5
glm::vec3 compute_ios_direction(const IosCameraCalibration& calibration, const glm::vec2& uv)
{
    // The lookup table holds the relative radial magnification for n linearly spaced radii.
    // The first position corresponds to radius = 0
    // The last position corresponds to the largest radius found in the image.
    float reference_dimension_width{calibration.reference_dimension_width()};
    float reference_dimension_height{calibration.reference_dimension_height()};

    float lens_distortion_center_x{calibration.lens_distortion_center_x()};
    float lens_distortion_center_y{calibration.lens_distortion_center_y()};
    const vector<float>& lens_distortion_lookup_table{calibration.lens_distortion_lookup_table()};

    // uu and vv are u and v in the reference width/height dimension.
    float uu{uv.x * reference_dimension_width};
    float vv{(1.0f - uv.y) * reference_dimension_height};

    float delta_uu{uu - lens_distortion_center_x};
    float delta_vv{vv - lens_distortion_center_y};
    float r{std::sqrt(delta_uu * delta_uu + delta_vv * delta_vv)};

    // Find the largest r value possible.
    float delta_uu_max{std::max(lens_distortion_center_x, reference_dimension_width - lens_distortion_center_x)};
    float delta_vv_max{std::max(lens_distortion_center_y, reference_dimension_height - lens_distortion_center_y)};
    float r_max{std::sqrt(delta_uu_max * delta_uu_max + delta_vv_max * delta_vv_max)};

    float magnification{get_magnification(r, lens_distortion_lookup_table, r_max)};

    // calibrated_uu, vv are with lens distortion calibrated
    float calibrated_delta_uu{delta_uu * (1.0f + magnification)};
    float calibrated_delta_vv{delta_vv * (1.0f + magnification)};

    float calibrated_uu{lens_distortion_center_x + calibrated_delta_uu};
    float calibrated_vv{lens_distortion_center_y + calibrated_delta_vv};

    float fx{calibration.fx()};
    float fy{calibration.fy()};
    float ox{calibration.ox()};
    float oy{calibration.oy()};
    // fx, fy, ox, oy are large numbers, expecting values coming from the reference dimension.
    // So using uu and vv directly without converting them back to u and v.
    return glm::vec3{(calibrated_uu - ox) / fx, (calibrated_vv - oy) / fy, -1.0f};
}

// The inverse function of compute_ios_direction.
glm::vec2 compute_ios_uv(const IosCameraCalibration& calibration, const glm::vec3& direction)
{
    float reference_dimension_width{calibration.reference_dimension_width()};
    float reference_dimension_height{calibration.reference_dimension_height()};

    float lens_distortion_center_x{calibration.lens_distortion_center_x()};
    float lens_distortion_center_y{calibration.lens_distortion_center_y()};
    const vector<float>& inverse_lens_distortion_lookup_table{calibration.inverse_lens_distortion_lookup_table()};

    float x{direction.x / -direction.z};
    float y{direction.y / -direction.z};
    // float z{-1.0f}

    float fx{calibration.fx()};
    float fy{calibration.fy()};
    float ox{calibration.ox()};
    float oy{calibration.oy()};

    float calibrated_uu{fx * x + ox};
    float calibrated_vv{fy * y + oy};

    float calibrated_delta_uu{calibrated_uu - lens_distortion_center_x};
    float calibrated_delta_vv{calibrated_vv - lens_distortion_center_y};
    float r{std::sqrt(calibrated_delta_uu * calibrated_delta_uu + calibrated_delta_vv * calibrated_delta_vv)};

    // Find the largest r value possible.
    float delta_uu_max{std::max(lens_distortion_center_x, reference_dimension_width - lens_distortion_center_x)};
    float delta_vv_max{std::max(lens_distortion_center_y, reference_dimension_height - lens_distortion_center_y)};
    float r_max{std::sqrt(delta_uu_max * delta_uu_max + delta_vv_max * delta_vv_max)};

    float magnification{get_magnification(r, inverse_lens_distortion_lookup_table, r_max)};

    float delta_uu{calibrated_delta_uu * (1.0f + magnification)};
    float delta_vv{calibrated_delta_vv * (1.0f + magnification)};

    float uu{lens_distortion_center_x + delta_uu};
    float vv{lens_distortion_center_y + delta_vv};

    float u{uu / reference_dimension_width};
    float v{1.0f - vv / reference_dimension_height};

    return glm::vec2{u, v};
}
} // namespace rgbd
