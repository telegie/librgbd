from ._librgbd_ffi import lib
from .camera_calibration import NativeCameraCalibration
from .depth_decoder import DepthCodecType
from .utils import cast_np_array_to_pointer
from .yuv_frame import YuvFrame
import numpy as np
import glm


class NativeFileWriterConfig:
    def __init__(self):
        self.ptr = lib.rgbd_file_writer_config_ctor()

    def close(self):
        lib.rgbd_file_writer_config_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def set_framerate(self, framerate: int):
        lib.rgbd_file_writer_config_set_framerate(self.ptr, framerate)

    def set_samplerate(self, samplerate: int):
        lib.rgbd_file_writer_config_set_samplerate(self.ptr, samplerate)

    def set_depth_codec_type(self, depth_codec_type: DepthCodecType):
        lib.rgbd_file_writer_config_set_depth_codec_type(self.ptr, int(depth_codec_type))

    def set_depth_unit(self, depth_unit: float):
        lib.rgbd_file_writer_config_set_depth_unit(self.ptr, depth_unit)


class NativeFileWriter:
    def __init__(self, file_path, native_calibration: NativeCameraCalibration, native_config: NativeFileWriterConfig):
        self.ptr = lib.rgbd_file_writer_ctor_to_path(file_path.encode("utf8"), native_calibration.ptr, native_config.ptr)

    def close(self):
        lib.rgbd_file_writer_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def write_cover(self, yuv_frame: YuvFrame):
        lib.rgbd_file_writer_write_cover(self.ptr,
                                         yuv_frame.width,
                                         yuv_frame.height,
                                         cast_np_array_to_pointer(yuv_frame.y_channel),
                                         cast_np_array_to_pointer(yuv_frame.u_channel),
                                         cast_np_array_to_pointer(yuv_frame.v_channel))

    def write_video_frame(self, time_point_us: int, keyframe: bool,
                          color_bytes: np.ndarray, depth_bytes: np.ndarray):
        lib.rgbd_file_writer_write_video_frame(self.ptr, time_point_us, keyframe,
                                               cast_np_array_to_pointer(color_bytes), color_bytes.size,
                                               cast_np_array_to_pointer(depth_bytes), depth_bytes.size)

    def write_audio_frame(self, time_point_us: int, audio_bytes, audio_byte_size: int):
        lib.rgbd_file_writer_write_audio_frame(self.ptr, time_point_us, audio_bytes, audio_byte_size)

    def write_imu_frame(self, time_point_us: int,
                        acceleration: glm.vec3, rotation_rate: glm.vec3,
                        magnetic_field: glm.vec3, gravity: glm.vec3):
        lib.rgbd_file_writer_write_imu_frame(self.ptr, time_point_us,
                                             acceleration.x, acceleration.y, acceleration.z,
                                             rotation_rate.x, rotation_rate.y, rotation_rate.z,
                                             magnetic_field.x, magnetic_field.y, magnetic_field.z,
                                             gravity.x, gravity.y, gravity.z)
    def write_trs_frame(self, time_point_us: int,
                        translation: glm.vec3, rotation: glm.vec3, scale: glm.vec3):
        lib.rgbd_file_writer_write_trs_frame(self.ptr, time_point_us,
                                             translation.x, translation.y, translation.z,
                                             rotation.w, rotation.x, rotation.y, rotation.z,
                                             scale.x, scale.y, scale.z)

    def flush(self):
        lib.rgbd_file_writer_flush(self.ptr)
