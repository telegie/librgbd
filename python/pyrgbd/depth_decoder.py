from enum import IntEnum
from ._librgbd_ffi import lib
from .integer_frame import NativeInt32Frame, Int32Frame
import numpy as np
from .utils import cast_np_array_to_pointer


class DepthCodecType(IntEnum):
    RVL = lib.RGBD_DEPTH_CODEC_TYPE_RVL
    TDC1 = lib.RGBD_DEPTH_CODEC_TYPE_TDC1


class NativeDepthDecoder:
    def __init__(self, depth_codec_type: DepthCodecType):
        self.ptr = lib.rgbd_depth_decoder_ctor(int(depth_codec_type))

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
            depth_frame = Int32Frame.from_native(native_depth_frame)
        return depth_frame
