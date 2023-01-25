from ._librgbd_ffi import lib
from .integer_frame import NativeInt32Frame, Int32Frame
from .yuv_frame import NativeYuvFrame, YuvFrame
from .capi_containers import NativeByteArray
from .camera_calibration import CameraCalibration
import numpy as np


class NativeFrameMapper:
    def __init__(self, from_calibration: CameraCalibration, to_calibration):
        with from_calibration.to_native() as native_from_calibration, \
                to_calibration.to_native() as native_to_calibration:
            self.ptr = lib.rgbd_frame_mapper_ctor(native_from_calibration.ptr,
                                                  native_to_calibration.ptr)

    def close(self):
        lib.rgbd_frame_mapper_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def map_color_frame(self, color_frame: YuvFrame) -> YuvFrame:
        with color_frame.to_native() as native_color_frame:
            mapped_color_frame_ptr = lib.rgbd_frame_mapper_map_color_frame(self.ptr,
                                                                           native_color_frame.ptr)
            return YuvFrame.from_native(NativeYuvFrame(mapped_color_frame_ptr))

    def map_depth_frame(self, depth_frame: Int32Frame) -> Int32Frame:
        with depth_frame.to_native() as native_depth_frame:
            mapped_depth_frame_ptr = lib.rgbd_frame_mapper_map_depth_frame(self.ptr,
                                                                           native_depth_frame.ptr)
            return Int32Frame.from_native(NativeInt32Frame(mapped_depth_frame_ptr))
