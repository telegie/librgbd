#include "frame_mapper.hpp"

namespace rgbd
{
vector<optional<int>> get_index_map(const CameraCalibration& from_calibration,
                                    int from_width,
                                    int from_height,
                                    const CameraCalibration& to_calibration,
                                    int to_width,
                                    int to_height)
{
    // For mapping from from_calibration indices to to_calibration indices.
    vector<optional<int>> index_map(from_width * from_height);
    for (int from_row{0}; from_row < from_height; ++from_row) {
        for (int from_col{0}; from_col < from_width; ++from_col) {
            float from_u{from_col / static_cast<float>(from_width - 1)};
            float from_v{from_row / static_cast<float>(from_height - 1)};

            auto direction{from_calibration.getDirection(glm::vec2{from_u, from_v})};
            auto to_uv{to_calibration.getUv(direction)};
            int to_col{static_cast<int>(std::round(to_uv.x * (to_width - 1)))};
            int to_row{static_cast<int>(std::round(to_uv.y * (to_height - 1)))};

            int from_index{from_col + from_row * from_width};
            if (to_col < 0 || to_col >= to_width || to_row < 0 || to_row >= to_height) {
                index_map[from_index] = nullopt;
            } else {
                index_map[from_index] = to_col + to_row * to_width;
            }
        }
    }
    return index_map;
}

FrameMapper::FrameMapper(const rgbd::CameraCalibration& src_calibration,
                         const rgbd::CameraCalibration& dst_calibration)
    : dst_color_width_{dst_calibration.getColorWidth()}
    , dst_color_height_{dst_calibration.getColorHeight()}
    , dst_depth_width_{dst_calibration.getDepthWidth()}
    , dst_depth_height_{dst_calibration.getDepthHeight()}
    , y_index_map_{get_index_map(dst_calibration,
                                 dst_calibration.getColorWidth(),
                                 dst_calibration.getColorHeight(),
                                 src_calibration,
                                 src_calibration.getColorWidth(),
                                 src_calibration.getColorHeight())}
    , uv_index_map_{get_index_map(dst_calibration,
                                  dst_calibration.getColorWidth() / 2,
                                  dst_calibration.getColorHeight() / 2,
                                  src_calibration,
                                  src_calibration.getColorWidth() / 2,
                                  src_calibration.getColorHeight() / 2)}
    , depth_index_map_{get_index_map(dst_calibration,
                                     dst_calibration.getDepthWidth(),
                                     dst_calibration.getDepthHeight(),
                                     src_calibration,
                                     src_calibration.getDepthWidth(),
                                     src_calibration.getDepthHeight())}
{
}

unique_ptr<YuvFrame> FrameMapper::mapColorFrame(const YuvFrame& color_frame)
{
    vector<uint8_t> mapped_y_channel(dst_color_width_ * dst_color_height_);
    for (size_t i{0}; i < mapped_y_channel.size(); ++i) {
        auto index{y_index_map_[i]};
        if (!index) {
            // Painting missing pixels black.
            // Black corresponds to y = 0, u = 128, v = 128.
            mapped_y_channel[i] = 0;
        } else {
            mapped_y_channel[i] = color_frame.y_channel()[*index];
        }
    }
    vector<uint8_t> mapped_u_channel(mapped_y_channel.size() / 4);
    vector<uint8_t> mapped_v_channel(mapped_y_channel.size() / 4);
    for (size_t i{0}; i < mapped_u_channel.size(); ++i) {
        auto index{uv_index_map_[i]};
        if (!index) {
            // Painting missing pixels black.
            // Black corresponds to y = 0, u = 128, v = 128.
            mapped_u_channel[i] = 128;
            mapped_v_channel[i] = 128;
        } else {
            mapped_u_channel[i] = color_frame.u_channel()[*index];
            mapped_v_channel[i] = color_frame.v_channel()[*index];
        }
    }

    return std::make_unique<YuvFrame>(dst_color_width_,
                                      dst_color_height_,
                                      std::move(mapped_y_channel),
                                      std::move(mapped_u_channel),
                                      std::move(mapped_v_channel));
}

unique_ptr<Int32Frame> FrameMapper::mapDepthFrame(const Int32Frame& depth_frame)
{
    vector<int> mapped_depth_values(dst_depth_width_ * dst_depth_height_);
    for (size_t i{0}; i < mapped_depth_values.size(); ++i) {
        auto index{depth_index_map_[i]};
        if (!index) {
            mapped_depth_values[i] = 0;
        } else {
            mapped_depth_values[i] = depth_frame.values()[*index];
        }
    }

    return std::make_unique<Int32Frame>(dst_depth_width_, dst_depth_height_, mapped_depth_values);
}
} // namespace rgbd