from .camera_calibration import CameraCalibration
from .depth_decoder import DepthCodecType
from .file import FileVideoFrame, FileAudioFrame, FileIMUFrame, FileTRSFrame
from .file_writer import NativeFileWriterConfig, NativeFileWriter
from .yuv_frame import YuvFrame
from .utils import cast_np_array_to_pointer


class VideoByteFrame:
    def __init__(self, time_point_us, keyframe, bytes):
        self.time_point_us = time_point_us
        self.keyframe = keyframe
        self.bytes = bytes


class FileWriterHelper:
    def __init__(self):
        self.calibration = None
        self.depth_codec_type = DepthCodecType.TDC1
        self.depth_unit = None
        self.cover = None
        self.color_byte_frames = []
        self.depth_byte_frames = []
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

    def add_color_byte_frame(self, time_point_us, keyframe, color_bytes):
        self.color_byte_frames.append(VideoByteFrame(time_point_us, keyframe, color_bytes))

    def add_depth_byte_frame(self, time_point_us, keyframe, depth_bytes):
        self.depth_byte_frames.append(VideoByteFrame(time_point_us, keyframe, depth_bytes))

    def add_audio_frame(self, audio_frame: FileAudioFrame):
        self.audio_frames.append(audio_frame)

    def add_imu_frame(self, imu_frame: FileIMUFrame):
        self.imu_frames.append(imu_frame)

    def add_trs_frame(self, trs_frame: FileTRSFrame):
        self.trs_frames.append(trs_frame)

    def write(self, output_file_path):
        if len(self.color_byte_frames) != len(self.depth_byte_frames):
            raise Exception("len(color_byte_frames) != len(depth_byte_frames) from FileWriterHelper")

        self.color_byte_frames.sort(key=lambda x: x.time_point_us)
        self.depth_byte_frames.sort(key=lambda x: x.time_point_us)
        self.audio_frames.sort(key=lambda x: x.time_point_us)
        self.imu_frames.sort(key=lambda x: x.time_point_us)
        self.trs_frames.sort(key=lambda x: x.time_point_us)

        # Fill in missing time_point_us or keyframe values of color_byte_frames.
        for video_frame_index in range(len(self.color_byte_frames)):
            color_byte_frame = self.color_byte_frames[video_frame_index]
            depth_byte_frame = self.depth_byte_frames[video_frame_index]

            if color_byte_frame.time_point_us is None:
                raise Exception("color_byte_frame.time_point_us is None")
            if depth_byte_frame.time_point_us is None:
                raise Exception("depth_byte_frame.time_point_us is None")
            if color_byte_frame.time_point_us != depth_byte_frame.time_point_us:
                raise Exception("color_byte_frame.time_point_us != depth_byte_frame.time_point_us from FileWriterHelper")

            if color_byte_frame.keyframe is None:
                raise Exception("color_byte_frame.keyframe is None")
            if depth_byte_frame.keyframe is None:
                raise Exception("depth_byte_frame.keyframe is None")
            if color_byte_frame.keyframe != depth_byte_frame.keyframe:
                raise Exception("color_byte_frame.keyframe != depth_byte_frame.keyframe from FileWriterHelper")

        # Find minimum_time_point_us.
        initial_time_points = []
        if len(self.color_byte_frames) > 0:
            initial_time_points.append(self.color_byte_frames[0].time_point_us)
        if len(self.audio_frames) > 0:
            initial_time_points.append(self.audio_frames[0].time_point_us)
        if len(self.imu_frames) > 0:
            initial_time_points.append(self.imu_frames[0].time_point_us)
        if len(self.trs_frames) > 0:
            initial_time_points.append(self.trs_frames[0].time_point_us)
        if len(initial_time_points) == 0:
            raise Exception("No frame found from FileWriterHelper")
        minimum_time_point_us = min(initial_time_points)

        if self.calibration == None:
            raise Exception("No CameraCalibration found from FileWriterHelper")

        with self.calibration.to_native() as native_calibration:
            write_config = NativeFileWriterConfig()
            write_config.set_depth_codec_type(self.depth_codec_type)
            if self.depth_unit is not None:
                write_config.set_depth_unit(self.depth_unit)
            file_writer = NativeFileWriter(output_file_path,
                                           native_calibration,
                                           write_config)

        file_writer.write_cover(self.cover)

        audio_frame_index = 0
        imu_frame_index = 0
        trs_frame_index = 0
        for video_frame_index in range(len(self.color_byte_frames)):
            color_byte_frame = self.color_byte_frames[video_frame_index]
            depth_byte_frame = self.depth_byte_frames[video_frame_index]

            video_time_point_us = color_byte_frame.time_point_us

            # Write audio frames fitting in front of the video frame.
            while audio_frame_index < len(self.audio_frames):
                audio_frame = self.audio_frames[audio_frame_index]
                if audio_frame.time_point_us > video_time_point_us:
                    break
                file_writer.write_audio_frame(audio_frame.time_point_us - minimum_time_point_us,
                                              cast_np_array_to_pointer(audio_frame.bytes),
                                              audio_frame.bytes.size)
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

            file_writer.write_video_frame(video_time_point_us - minimum_time_point_us,
                                          color_byte_frame.keyframe,
                                          color_byte_frame.bytes,
                                          depth_byte_frame.bytes)

        file_writer.flush()
        print(f"Wrote file to {output_file_path}")
