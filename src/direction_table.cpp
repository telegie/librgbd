/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "direction_table.hpp"

#include <cmath>

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
    int top_row{static_cast<int>(std::floor(row))};

    int left_top_index{left_col + top_row * width_};
    glm::vec3 left_top_direction{directions_[left_top_index]};
    glm::vec3 right_top_direction{directions_[left_top_index + 1]};
    glm::vec3 left_bottom_direction{directions_[left_top_index + width_]};

    glm::vec3 horizontal_diff{right_top_direction - left_top_direction};
    glm::vec3 vertical_diff{right_top_direction - left_top_direction};
    float col_remainder = col - left_col;
    float row_remainder = row - top_row;

    glm::vec3 direction{left_top_direction + horizontal_diff * col_remainder + vertical_diff * row_remainder};
    return direction;
}
} // namespace rgbd
