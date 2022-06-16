#pragma once

#include <glm/glm.hpp>
#include "constants.hpp"

namespace rgbd
{
class CameraCalibration
{
public:
    virtual ~CameraCalibration() {}
    virtual json toJson() const noexcept = 0;
    virtual CameraDeviceType getCameraDeviceType() const noexcept = 0;
    virtual int getColorWidth() const noexcept = 0;
    virtual int getColorHeight() const noexcept = 0;
    virtual int getDepthWidth() const noexcept = 0;
    virtual int getDepthHeight() const noexcept = 0;
    virtual glm::vec3 getDirection(const glm::vec2& uv) const noexcept = 0;
};
}
