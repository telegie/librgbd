from ._librgbd_ffi import lib
from .yuv_frame import NativeYuvFrame, YuvFrame
from .capi_containers import NativeByteArray
from .utils import cast_np_array_to_pointer
import numpy as np


class NativeColorDecoder:
    def __init__(self):
        # Setting lib.VP8 assuming since it is the only codec for now.
        # Fix this later when a codec gets added.
        self.ptr = lib.rgbd_color_decoder_ctor(lib.RGBD_COLOR_CODEC_TYPE_VP8)

    def close(self):
        lib.rgbd_color_decoder_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def decode(self, color_frame_bytes: np.ndarray) -> YuvFrame:
        native_yuv_frame_ptr = lib.rgbd_color_decoder_decode(self.ptr,
                                                             cast_np_array_to_pointer(color_frame_bytes),
                                                             color_frame_bytes.size)
        with NativeYuvFrame(native_yuv_frame_ptr) as native_yuv_frame:
            yuv_frame = YuvFrame.from_native(native_yuv_frame)
        return yuv_frame


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
