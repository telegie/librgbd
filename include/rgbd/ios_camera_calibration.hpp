#pragma once

#include <rgbd/byte_utils.hpp>
#include "camera_calibration.hpp"

namespace rgbd
{
class IosCameraCalibration : public CameraCalibration
{
public:
    IosCameraCalibration(int color_width,
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
                         span<const float> lens_distortion_lookup_table,
                         span<const float> inverse_lens_distortion_lookup_table);
    unique_ptr<CameraCalibration> clone() const noexcept;
    static IosCameraCalibration fromBytes(const Bytes& bytes, int& cursor);
    static IosCameraCalibration fromJson(const json& json);
    Bytes toBytes() const noexcept;
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
    float ox() const noexcept
    {
        return ox_;
    }
    float oy() const noexcept
    {
        return oy_;
    }
    float reference_dimension_width() const noexcept
    {
        return reference_dimension_width_;
    }
    float reference_dimension_height() const noexcept
    {
        return reference_dimension_height_;
    }
    float lens_distortion_center_x() const noexcept
    {
        return lens_distortion_center_x_;
    }
    float lens_distortion_center_y() const noexcept
    {
        return lens_distortion_center_y_;
    }
    const vector<float> lens_distortion_lookup_table() const noexcept
    {
        return lens_distortion_lookup_table_;
    }
    const vector<float> inverse_lens_distortion_lookup_table() const noexcept
    {
        return inverse_lens_distortion_lookup_table_;
    }

private:
    int color_width_;
    int color_height_;
    int depth_width_;
    int depth_height_;

    // intrinsics
    float fx_;
    float fy_;
    float ox_;
    float oy_;
    float reference_dimension_width_;
    float reference_dimension_height_;
    float lens_distortion_center_x_;
    float lens_distortion_center_y_;
    vector<float> lens_distortion_lookup_table_;
    vector<float> inverse_lens_distortion_lookup_table_;
};
} // namespace rgbd
