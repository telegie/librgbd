#pragma once

#include "camera_calibration.hpp"
#include "integer_frame.hpp"
#include "yuv_frame.hpp"

namespace rgbd
{
class FrameMapper
{
public:
    FrameMapper(const rgbd::CameraCalibration& src_calibration,
                const rgbd::CameraCalibration& dst_calibration);
    YuvFrame mapColorFrame(const YuvFrame& yuv_frame);
    Int32Frame mapDepthFrame(const Int32Frame& depth_frame);

private:
    int dst_color_width_;
    int dst_color_height_;
    int dst_depth_width_;
    int dst_depth_height_;
    vector<optional<int>> y_index_map_;
    vector<optional<int>> uv_index_map_;
    vector<optional<int>> depth_index_map_;
};
}
