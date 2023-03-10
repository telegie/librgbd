from ._librgbd_ffi import ffi, lib
import glm
from .capi_containers import NativeQuaternion, NativeVector3


def apply_rotation_rate_and_gravity_to_rotation(rotation: glm.quat,
                                                delta_time_sec: float,
                                                rotation_rate: glm.vec3,
                                                gravity: glm.vec3) -> glm.quat:
    native_rotation = NativeQuaternion.from_glm(rotation)
    native_rotation_rate = NativeVector3.from_glm(rotation_rate)
    native_gravity = NativeVector3.from_glm(gravity)
    result_ptr = lib.rgbd_math_utils_apply_rotation_rate_and_gravity_to_rotation(native_rotation.ptr,
                                                                                 delta_time_sec,
                                                                                 native_rotation_rate.ptr,
                                                                                 native_gravity.ptr)
    native_result = NativeQuaternion(result_ptr)
    result = native_result.to_glm()
    return result
