/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "direction_table.hpp"

#include <cmath>
#include <spdlog/spdlog.h>

namespace rgbd
{
DirectionTable::DirectionTable(int width, int height, const std::vector<glm::vec3>& directions)
    : width_{width}
    , height_{height}
    , directions_{directions}
{
}
DirectionTable::DirectionTable(const rgbd::CameraCalibration& calibration)
    : width_{calibration.getDepthWidth()}
    , height_{calibration.getDepthHeight()}
    , directions_{}
{
    directions_.resize(gsl::narrow<size_t>(width_ * height_));

    float u_scale{1.0f / (width_ - 1)};
    float v_scale{1.0f / (height_ - 1)};
    for (int row{0}; row < height_; ++row) {
        for (int col{0}; col < width_; ++col) {
            float u{col * u_scale};
            float v{row * v_scale};
            directions_[col + row * width_] = calibration.getDirection(glm::vec2{u, v});
        }
    }
}

glm::vec3 DirectionTable::getDirection(const glm::vec2& uv) const
{
    // The row and col aiming to obtain,
    // and will be obtained through interpolation.
    float col{uv.x * (width_ - 1)};
    float row{uv.y * (height_ - 1)};

    int left_col{static_cast<int>(std::floor(col))};
    int right_col{left_col + 1};
    int top_row{static_cast<int>(std::floor(row))};
    int bottom_row{top_row + 1};

    // For vertices at the right or bottom edge.
    right_col = glm::min(right_col, width_ - 1);
    bottom_row = glm::min(bottom_row, height_ - 1);

    glm::vec3 left_top{directions_[left_col + top_row * width_]};
    glm::vec3 right_top{directions_[right_col + top_row * width_]};
    glm::vec3 left_bottom{directions_[left_col + bottom_row * width_]};
    glm::vec3 right_bottom{directions_[right_col + bottom_row * width_]};

    // Bilinear interpolation
    // ref: https://en.wikipedia.org/wiki/Bilinear_interpolation
    // ref: https://math.stackexchange.com/questions/3230376/interpolate-between-4-points-on-a-2d-plane
    float x = col - left_col; // col_remainder but using x to match the formula in above ref
    float y = row - top_row; // row_remainder but using y to match the formula in above ref
    glm::vec3 direction{(1.0f - x) * (1.0f - y) * left_top + x * (1.0f - y) * right_top +
                        (1.0f - x) * y * left_bottom + x * y * right_bottom};
    return direction;
}
} // namespace rgbd
