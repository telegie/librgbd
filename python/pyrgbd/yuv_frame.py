from ._librgbd_ffi import lib
from .capi_containers import NativeUInt8Array
from .utils import cast_np_array_to_pointer
import numpy as np


class NativeYuvFrame:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_yuv_frame_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_y_channel(self) -> NativeUInt8Array:
        return NativeUInt8Array(lib.rgbd_yuv_frame_get_y_channel(self.ptr))

    def get_u_channel(self) -> NativeUInt8Array:
        return NativeUInt8Array(lib.rgbd_yuv_frame_get_u_channel(self.ptr))

    def get_v_channel(self) -> NativeUInt8Array:
        return NativeUInt8Array(lib.rgbd_yuv_frame_get_v_channel(self.ptr))

    def get_width(self) -> int:
        return lib.rgbd_yuv_frame_get_width(self.ptr)

    def get_height(self) -> int:
        return lib.rgbd_yuv_frame_get_height(self.ptr)


class YuvFrame:
    def __init__(self, width: int, height: int,
                 y_channel: np.ndarray, u_channel: np.ndarray, v_channel: np.ndarray):
        self.width = width
        self.height = height
        self.y_channel = y_channel
        self.u_channel = u_channel
        self.v_channel = v_channel

    @classmethod
    def from_native(cls, native_yuv_frame: NativeYuvFrame):
        width = native_yuv_frame.get_width()
        height = native_yuv_frame.get_height()

        y_channel = native_yuv_frame.get_y_channel().to_np_array()
        y_channel = y_channel.reshape((height, width))

        u_channel = native_yuv_frame.get_u_channel().to_np_array()
        u_channel = u_channel.reshape((height // 2, width // 2))

        v_channel = native_yuv_frame.get_v_channel().to_np_array()
        v_channel = v_channel.reshape((height // 2, width // 2))
        return YuvFrame(width, height, y_channel, u_channel, v_channel)

    def to_native(self) -> NativeYuvFrame:
        print("??")
        ptr = lib.rgbd_yuv_frame_ctor(self.width,
                                      self.height,
                                      cast_np_array_to_pointer(self.y_channel),
                                      cast_np_array_to_pointer(self.u_channel),
                                      cast_np_array_to_pointer(self.v_channel))
        print("!!")
        return NativeYuvFrame(ptr)
