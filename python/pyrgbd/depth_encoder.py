from ._librgbd_ffi import lib
from .capi_containers import NativeByteArray
import numpy as np
from .utils import cast_np_array_to_pointer


class NativeDepthEncoder:
    def __init__(self, ptr):
        self.ptr = ptr

    @staticmethod
    def create_rvl_encoder(width: int, height: int):
        return NativeDepthEncoder(lib.rgbd_depth_encoder_create_rvl_encoder(width, height))

    @staticmethod
    def create_tdc1_encoder(width: int, height: int, depth_diff_multiplier: int):
        return NativeDepthEncoder(lib.rgbd_depth_encoder_create_tdc1_encoder(width, height, depth_diff_multiplier))

    def close(self):
        lib.rgbd_depth_encoder_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def encode(self, depth_values: np.ndarray, keyframe: bool) -> np.array:
        return NativeByteArray(
            lib.rgbd_depth_encoder_encode(self.ptr,
                                          cast_np_array_to_pointer(depth_values),
                                          keyframe)).to_np_array()
