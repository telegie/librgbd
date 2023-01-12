from ._librgbd_ffi import lib
from .integer_frame import NativeInt32Frame, Int32Frame
from .capi_containers import NativeByteArray
import numpy as np
from .utils import cast_np_array_to_pointer


class NativeDepthDecoder:
    def __init__(self, depth_codec_type):
        self.ptr = lib.rgbd_depth_decoder_ctor(depth_codec_type)

    def close(self):
        lib.rgbd_depth_decoder_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def decode(self, depth_frame_bytes: np.ndarray) -> Int32Frame:
        native_depth_frame_ptr = lib.rgbd_depth_decoder_decode(self.ptr,
                                                               cast_np_array_to_pointer(depth_frame_bytes),
                                                               depth_frame_bytes.size)
        with NativeInt32Frame(native_depth_frame_ptr) as native_depth_frame:
            depth_frame = Int32Frame(native_depth_frame)
        return depth_frame


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
