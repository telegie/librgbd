from ._librgbd_ffi import ffi, lib
import numpy as np
import glm


class NativeByteArray:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_native_byte_array_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_data(self):
        return lib.rgbd_native_byte_array_get_data(self.ptr)

    def get_size(self) -> int:
        return lib.rgbd_native_byte_array_get_size(self.ptr)

    def to_np_array(self) -> np.ndarray:
        buffer = ffi.buffer(self.get_data(), self.get_size())
        # np.frombuffer does not copy
        # std::byte is uint8_t, so using np.ubyte, not np.byte.
        np_array = np.frombuffer(buffer, dtype=np.ubyte)
        return np_array.copy()


class NativeFloatArray:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_native_float_array_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_data(self):
        return lib.rgbd_native_float_array_get_data(self.ptr)

    def get_size(self) -> int:
        return lib.rgbd_native_float_array_get_size(self.ptr)

    def to_np_array(self) -> np.ndarray:
        buffer = ffi.buffer(self.get_data(), self.get_size() * 4)
        # np.frombuffer does not copy
        np_array = np.frombuffer(buffer, dtype=np.float32)
        return np_array.copy()


class NativeInt32Array:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_native_int32_array_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_data(self):
        return lib.rgbd_native_int32_array_get_data(self.ptr)

    def get_size(self) -> int:
        return lib.rgbd_native_int32_array_get_size(self.ptr)

    def to_np_array(self) -> np.ndarray:
        # Multiplying 4 since int32 is 2 bytes and the second argument
        # is for the byte size.
        buffer = ffi.buffer(self.get_data(), self.get_size() * 4)
        # np.frombuffer does not copy, so returning a copy.
        np_array = np.frombuffer(buffer, dtype=np.int32)
        return np_array.copy()


class NativeQuaternion:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_native_quaternion_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    @classmethod
    def from_glm(self, quat: glm.quat):
        ptr = lib.rgbd_native_quaternion_ctor(quat.w, quat.x, quat.y, quat.z)
        return NativeQuaternion(ptr)

    def to_glm(self) -> glm.quat:
        return glm.quat(self.get_w(), self.get_y(), self.get_x(), self.get_z())

    def get_w(self) -> float:
        return lib.rgbd_native_quaternion_get_w(self.ptr)

    def get_x(self) -> float:
        return lib.rgbd_native_quaternion_get_x(self.ptr)

    def get_y(self) -> float:
        return lib.rgbd_native_quaternion_get_y(self.ptr)

    def get_z(self) -> float:
        return lib.rgbd_native_quaternion_get_z(self.ptr)


class NativeUInt8Array:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_native_uint8_array_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_data(self):
        return lib.rgbd_native_uint8_array_get_data(self.ptr)

    def get_size(self) -> int:
        return lib.rgbd_native_uint8_array_get_size(self.ptr)

    def to_np_array(self) -> np.ndarray:
        buffer = ffi.buffer(self.get_data(), self.get_size())
        # np.frombuffer does not copy
        np_array = np.frombuffer(buffer, dtype=np.uint8)
        return np_array.copy()


class NativeVector3:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_native_vector3_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    @classmethod
    def from_glm(cls, vec3: glm.vec3):
        ptr = lib.rgbd_native_vector3_ctor(vec3.x, vec3.y, vec3.z)
        return NativeVector3(ptr)

    def to_glm(self) -> glm.vec3:
        return glm.vec3(self.get_y(), self.get_x(), self.get_z())

    def get_x(self) -> float:
        return lib.rgbd_native_quaternion_get_x(self.ptr)

    def get_y(self) -> float:
        return lib.rgbd_native_quaternion_get_y(self.ptr)

    def get_z(self) -> float:
        return lib.rgbd_native_quaternion_get_z(self.ptr)
