#include "constants.hpp"

namespace rgbd
{
string stringify_camera_device_type(const CameraDeviceType& camera_device_type)
{
    switch (camera_device_type) {
    case CameraDeviceType::AzureKinect:
        return "AzureKinect";
    case CameraDeviceType::IOS:
        return "IOS";
    case CameraDeviceType::Undistorted:
        return "Undistorted";
    }
    throw std::runtime_error(fmt::format(
        "Invalid CameraDeviceType ({}) in stringify_camera_device_type.", camera_device_type));
}
}
