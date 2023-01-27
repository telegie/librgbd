from .camera_calibration import CameraCalibration
from .depth_decoder import DepthCodecType
from .file import FileVideoFrame, FileAudioFrame, FileIMUFrame, FileTRSFrame
from .file_writer import NativeFileWriterConfig, NativeFileWriter
from .yuv_frame import YuvFrame
from ._librgbd_ffi import lib


class NativeFileWriterHelper:
    def __init__(self):
        self.ptr = lib.rgbd_file_writer_helper_ctor()

    def close(self):
        lib.rgbd_file_writer_helper_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def set_calibration(self, calibration: CameraCalibration):
        with calibration.to_native() as native_calibration:
            lib.rgbd_file_writer_helper_set_calibration(self.ptr, native_calibration.ptr)

    def set_depth_codec_type(self, depth_codec_type: DepthCodecType):
        lib.rgbd_file_writer_helper_set_depth_codec_type(self.ptr, depth_codec_type)

    def set_depth_unit(self, depth_unit: float):
        lib.rgbd_file_writer_helper_set_depth_unit(self.ptr, depth_unit)

    def set_cover(self, cover: YuvFrame):
        with cover.to_native() as native_cover:
            lib.rgbd_file_writer_helper_set_cover(self.ptr, native_cover.ptr)

    def add_video_frame(self, video_frame: FileVideoFrame):
        with video_frame.to_native() as native_video_frame:
            lib.rgbd_file_writer_helper_add_video_frame(self.ptr, native_video_frame.ptr)

    def add_audio_frame(self, audio_frame: FileAudioFrame):
        with audio_frame.to_native() as native_audio_frame:
            lib.rgbd_file_writer_helper_add_audio_frame(self.ptr, native_audio_frame.ptr)

    def add_imu_frame(self, imu_frame: FileIMUFrame):
        with imu_frame.to_native() as native_imu_frame:
            lib.rgbd_file_writer_helper_add_imu_frame(self.ptr, native_imu_frame.ptr)

    def add_trs_frame(self, trs_frame: FileTRSFrame):
        with trs_frame.to_native() as native_trs_frame:
            lib.rgbd_file_writer_helper_add_trs_frame(self.ptr, native_trs_frame.ptr)

    def write_to_path(self, path: str):
        lib.rgbd_file_writer_helper_write_to_path(self.ptr, path.encode("utf8"))
