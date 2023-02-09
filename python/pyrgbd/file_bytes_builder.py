from .camera_calibration import CameraCalibration
from .depth_decoder import DepthCodecType
from .file import FileVideoFrame, FileAudioFrame, FileIMUFrame, FileTRSFrame
from .yuv_frame import YuvFrame
from ._librgbd_ffi import lib
import numpy as np
from .utils import cast_np_array_to_pointer


class NativeFileBytesBuilder:
    def __init__(self):
        self.ptr = lib.rgbd_file_bytes_builder_ctor()

    def close(self):
        lib.rgbd_file_bytes_builder_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def set_calibration(self, calibration: CameraCalibration):
        with calibration.to_native() as native_calibration:
            lib.rgbd_file_bytes_builder_set_calibration(
                self.ptr, native_calibration.ptr
            )

    def set_depth_codec_type(self, depth_codec_type: DepthCodecType):
        lib.rgbd_file_bytes_builder_set_depth_codec_type(self.ptr, depth_codec_type)

    def set_depth_unit(self, depth_unit: float):
        lib.rgbd_file_bytes_builder_set_depth_unit(self.ptr, depth_unit)

    def set_cover_png_bytes(self, cover_png_bytes: np.ndarray):
        lib.rgbd_file_bytes_builder_set_cover_png_bytes(
            self.ptr, cast_np_array_to_pointer(cover_png_bytes), cover_png_bytes.size
        )

    def add_video_frame(self, video_frame: FileVideoFrame):
        with video_frame.to_native() as native_video_frame:
            lib.rgbd_file_bytes_builder_add_video_frame(
                self.ptr, native_video_frame.ptr
            )

    def add_audio_frame(self, audio_frame: FileAudioFrame):
        with audio_frame.to_native() as native_audio_frame:
            lib.rgbd_file_bytes_builder_add_audio_frame(
                self.ptr, native_audio_frame.ptr
            )

    def add_imu_frame(self, imu_frame: FileIMUFrame):
        with imu_frame.to_native() as native_imu_frame:
            lib.rgbd_file_bytes_builder_add_imu_frame(self.ptr, native_imu_frame.ptr)

    def add_trs_frame(self, trs_frame: FileTRSFrame):
        with trs_frame.to_native() as native_trs_frame:
            lib.rgbd_file_bytes_builder_add_trs_frame(self.ptr, native_trs_frame.ptr)

    def build_to_path(self, path: str):
        lib.rgbd_file_bytes_builder_build_to_path(self.ptr, path.encode("utf8"))
