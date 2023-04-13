#include "constants.hpp"

namespace rgbd
{
string stringify_camera_calibration_type(const CameraCalibrationType& camera_calibration_type)
{
    switch (camera_calibration_type) {
    case CameraCalibrationType::AzureKinect:
        return "AzureKinect";
    case CameraCalibrationType::IOS:
        return "IOS";
    case CameraCalibrationType::Undistorted:
        return "Undistorted";
    }
    throw std::runtime_error(
        fmt::format("Invalid CameraCalibrationType ({}) in stringify_camera_calibration_type.",
                    camera_calibration_type));
}
} // namespace rgbd
