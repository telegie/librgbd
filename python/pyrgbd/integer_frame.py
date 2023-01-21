import numpy as np
from ._librgbd_ffi import lib
from .capi_containers import NativeInt32Array


class NativeInt32Frame:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_int32_frame_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_width(self) -> int:
        return lib.rgbd_int32_frame_get_width(self.ptr)

    def get_height(self) -> int:
        return lib.rgbd_int32_frame_get_height(self.ptr)

    def get_values(self) -> NativeInt32Array:
        return NativeInt32Array(lib.rgbd_int32_frame_get_values(self.ptr))


class Int32Frame:
    def __init__(self, width: int, height: int, values: np.array):
        self.width = width
        self.height = height
        self.values = values

    @classmethod
    def from_native(self, native_int32_frame: NativeInt32Frame):
        width = native_int32_frame.get_width()
        height = native_int32_frame.get_height()
        values = native_int32_frame.get_values().to_np_array().reshape((height, width))
        return Int32Frame(width, height, values)
