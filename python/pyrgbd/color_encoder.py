from ._librgbd_ffi import lib
from .yuv_frame import YuvFrame
from .capi_containers import NativeByteArray
from .utils import cast_np_array_to_pointer
import numpy as np


class NativeColorEncoder:
    def __init__(self, color_codec_type, width: int, height: int, target_bitrate: int, framerate: int):
        # Setting lib.VP8 assuming since it is the only codec for now.
        # Fix this later when a codec gets added.
        self.ptr = lib.rgbd_color_encoder_ctor(color_codec_type,
                                               width,
                                               height,
                                               target_bitrate,
                                               framerate)

    def close(self):
        lib.rgbd_color_encoder_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def encode(self, yuv_frame: YuvFrame, keyframe) -> np.array:
        return NativeByteArray(lib.rgbd_color_encoder_encode(self.ptr,
                                                             cast_np_array_to_pointer(yuv_frame.y_channel),
                                                             cast_np_array_to_pointer(yuv_frame.u_channel),
                                                             cast_np_array_to_pointer(yuv_frame.v_channel),
                                                             keyframe)).to_np_array()
