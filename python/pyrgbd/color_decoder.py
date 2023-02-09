from enum import IntEnum
from ._librgbd_ffi import lib
from .yuv_frame import NativeYuvFrame, YuvFrame
from .utils import cast_np_array_to_pointer
import numpy as np


class ColorCodecType(IntEnum):
    VP8 = lib.RGBD_COLOR_CODEC_TYPE_VP8


class NativeColorDecoder:
    def __init__(self, color_codec_type: ColorCodecType):
        # Setting lib.VP8 assuming since it is the only codec for now.
        # Fix this later when a codec gets added.
        self.ptr = lib.rgbd_color_decoder_ctor(color_codec_type)

    def close(self):
        lib.rgbd_color_decoder_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def decode(self, color_frame_bytes: np.ndarray) -> YuvFrame:
        native_yuv_frame_ptr = lib.rgbd_color_decoder_decode(
            self.ptr,
            cast_np_array_to_pointer(color_frame_bytes),
            color_frame_bytes.size,
        )
        with NativeYuvFrame(native_yuv_frame_ptr) as native_yuv_frame:
            yuv_frame = YuvFrame.from_native(native_yuv_frame)
        return yuv_frame
