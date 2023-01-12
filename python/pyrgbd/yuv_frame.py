from ._librgbd_ffi import lib
from .capi_containers import NativeUInt8Array
import numpy as np
from typing import TypeVar


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


YuvFrameT = TypeVar('YuvFrameT', bound='YuvFrame')


class YuvFrame:
    def __init__(self, width: int, height: int,
                 y_channel: np.ndarray, u_channel: np.ndarray, v_channel: np.ndarray):
        self.width = width
        self.height = height
        self.y_channel = y_channel
        self.u_channel = u_channel
        self.v_channel = v_channel

    @staticmethod
    def from_native(native_yuv_frame: NativeYuvFrame) -> YuvFrameT:
        width = native_yuv_frame.get_width()
        height = native_yuv_frame.get_height()

        y_channel = native_yuv_frame.get_y_channel().to_np_array()
        y_channel = y_channel.reshape((height, width))

        u_channel = native_yuv_frame.get_u_channel().to_np_array()
        u_channel = u_channel.reshape((height // 2, width // 2))

        v_channel = native_yuv_frame.get_v_channel().to_np_array()
        v_channel = v_channel.reshape((height // 2, width // 2))
        return YuvFrame(width, height, y_channel, u_channel, v_channel)
