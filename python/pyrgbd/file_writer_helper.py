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


class FileWriterHelper:
    def __init__(self):
        self.calibration = None
        self.depth_codec_type = DepthCodecType.TDC1
        self.depth_unit = None
        self.cover = None
        self.video_frames = []
        self.audio_frames = []
        self.imu_frames = []
        self.trs_frames = []

    def set_calibration(self, calibration: CameraCalibration):
        self.calibration = calibration

    def set_depth_codec_type(self, depth_codec_type: DepthCodecType):
        self.depth_codec_type = depth_codec_type

    def set_depth_unit(self, depth_unit: float):
        self.depth_unit = depth_unit

    def set_cover(self, cover: YuvFrame):
        self.cover = cover

    def add_video_frame(self, video_frame: FileVideoFrame):
        self.video_frames.append(video_frame)

    def add_audio_frame(self, audio_frame: FileAudioFrame):
        self.audio_frames.append(audio_frame)

    def add_imu_frame(self, imu_frame: FileIMUFrame):
        self.imu_frames.append(imu_frame)

    def add_trs_frame(self, trs_frame: FileTRSFrame):
        self.trs_frames.append(trs_frame)

    def write(self, output_file_path):
        self.video_frames.sort(key=lambda x: x.time_point_us)
        self.audio_frames.sort(key=lambda x: x.time_point_us)
        self.imu_frames.sort(key=lambda x: x.time_point_us)
        self.trs_frames.sort(key=lambda x: x.time_point_us)

        # Find minimum_time_point_us.
        initial_time_points = []
        if len(self.video_frames) > 0:
            initial_time_points.append(self.video_frames[0].time_point_us)
        if len(self.audio_frames) > 0:
            initial_time_points.append(self.audio_frames[0].time_point_us)
        if len(self.imu_frames) > 0:
            initial_time_points.append(self.imu_frames[0].time_point_us)
        if len(self.trs_frames) > 0:
            initial_time_points.append(self.trs_frames[0].time_point_us)
        if len(initial_time_points) == 0:
            raise Exception("No frame found from FileWriterHelper")
        minimum_time_point_us = min(initial_time_points)

        if self.calibration is None:
            raise Exception("No CameraCalibration found from FileWriterHelper")

        writer_config = NativeFileWriterConfig()
        writer_config.set_depth_codec_type(self.depth_codec_type)
        if self.depth_unit is not None:
            writer_config.set_depth_unit(self.depth_unit)
        file_writer = NativeFileWriter(output_file_path,
                                       self.calibration,
                                       writer_config)

        if self.cover is not None:
            file_writer.write_cover(self.cover)

        audio_frame_index = 0
        imu_frame_index = 0
        trs_frame_index = 0
        for video_frame in self.video_frames:
            video_time_point_us = video_frame.time_point_us

            # Write audio frames fitting in front of the video frame.
            while audio_frame_index < len(self.audio_frames):
                audio_frame = self.audio_frames[audio_frame_index]
                if audio_frame.time_point_us > video_time_point_us:
                    break
                file_writer.write_audio_frame(audio_frame.time_point_us - minimum_time_point_us,
                                              audio_frame.bytes)
                audio_frame_index = audio_frame_index + 1

            # Write IMU frames fitting in front of the video frame.
            while imu_frame_index < len(self.imu_frames):
                imu_frame = self.imu_frames[imu_frame_index]
                if imu_frame.time_point_us > video_time_point_us:
                    break
                file_writer.write_imu_frame(imu_frame.time_point_us - minimum_time_point_us,
                                            imu_frame.acceleration,
                                            imu_frame.rotation_rate,
                                            imu_frame.magnetic_field,
                                            imu_frame.gravity)
                imu_frame_index = imu_frame_index + 1

            # Write TRS frames fitting in front of the video frame.
            while trs_frame_index < len(self.trs_frames):
                trs_frame = self.trs_frames[trs_frame_index]
                if trs_frame.time_point_us > video_time_point_us:
                    break
                file_writer.write_trs_frame(trs_frame.time_point_us - minimum_time_point_us,
                                            trs_frame.translation,
                                            trs_frame.rotation,
                                            trs_frame.scale)
                trs_frame_index = trs_frame_index + 1

            file_writer.write_video_frame(video_frame.time_point_us - minimum_time_point_us,
                                          video_frame.keyframe,
                                          video_frame.color_bytes,
                                          video_frame.depth_bytes)

        file_writer.flush()
        print(f"Wrote file to {output_file_path}")
