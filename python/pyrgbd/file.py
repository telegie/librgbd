import numpy as np
import glm
from ._librgbd_ffi import lib
from .capi_containers import NativeByteArray
from .camera_calibration import NativeCameraCalibration, CameraCalibration
from .direction_table import NativeDirectionTable, DirectionTable
from typing import Optional
from .color_decoder import ColorCodecType
from .depth_decoder import DepthCodecType
from .utils import cast_np_array_to_pointer


class NativeFileInfo:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_file_info_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_duration_us(self) -> float:
        return lib.rgbd_file_info_get_duration_us(self.ptr)


class NativeFileVideoTrack:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_file_video_track_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_track_number(self) -> int:
        return lib.rgbd_file_video_track_get_track_number(self.ptr)

    def get_width(self) -> int:
        return lib.rgbd_file_video_track_get_width(self.ptr)

    def get_height(self) -> int:
        return lib.rgbd_file_video_track_get_height(self.ptr)


class NativeFileColorVideoTrack(NativeFileVideoTrack):
    def get_codec(self) -> ColorCodecType:
        return ColorCodecType(lib.rgbd_file_color_video_track_get_codec(self.ptr))


class NativeFileDepthVideoTrack(NativeFileVideoTrack):
    def get_codec(self) -> DepthCodecType:
        return DepthCodecType(lib.rgbd_file_depth_video_track_get_codec(self.ptr))

    def get_depth_unit(self) -> float:
        return lib.rgbd_file_depth_video_track_get_depth_unit(self.ptr)


class NativeFileTracks:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_file_tracks_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_color_track(self) -> NativeFileColorVideoTrack:
        return NativeFileColorVideoTrack(lib.rgbd_file_tracks_get_color_track(self.ptr), False)

    def get_depth_track(self) -> NativeFileDepthVideoTrack:
        return NativeFileDepthVideoTrack(lib.rgbd_file_tracks_get_depth_track(self.ptr), False)


class NativeFileAttachments:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_file_attachments_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_camera_calibration(self) -> NativeCameraCalibration:
        return NativeCameraCalibration.create(lib.rgbd_file_attachments_get_camera_calibration(self.ptr), False)


class NativeFileVideoFrame:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_file_video_frame_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_time_point_us(self) -> int:
        return lib.rgbd_file_video_frame_get_time_point_us(self.ptr)

    def get_keyframe(self) -> bool:
        return lib.rgbd_file_video_frame_get_keyframe(self.ptr)

    def get_color_bytes(self) -> NativeByteArray:
        return NativeByteArray(lib.rgbd_file_video_frame_get_color_bytes(self.ptr))

    def get_depth_bytes(self) -> NativeByteArray:
        return NativeByteArray(lib.rgbd_file_video_frame_get_depth_bytes(self.ptr))


class NativeFileAudioFrame:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_file_audio_frame_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_time_point_us(self) -> int:
        return lib.rgbd_file_audio_frame_get_time_point_us(self.ptr)

    def get_bytes(self) -> NativeByteArray:
        return NativeByteArray(lib.rgbd_file_audio_frame_get_bytes(self.ptr))


class NativeFileIMUFrame:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_file_imu_frame_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_time_point_us(self) -> int:
        return lib.rgbd_file_imu_frame_get_time_point_us(self.ptr)

    def get_acceleration_x(self) -> float:
        return lib.rgbd_file_imu_frame_get_acceleration_x(self.ptr)

    def get_acceleration_y(self) -> float:
        return lib.rgbd_file_imu_frame_get_acceleration_y(self.ptr)

    def get_acceleration_z(self) -> float:
        return lib.rgbd_file_imu_frame_get_acceleration_z(self.ptr)

    def get_rotation_rate_x(self) -> float:
        return lib.rgbd_file_imu_frame_get_rotation_rate_x(self.ptr)

    def get_rotation_rate_y(self) -> float:
        return lib.rgbd_file_imu_frame_get_rotation_rate_y(self.ptr)

    def get_rotation_rate_z(self) -> float:
        return lib.rgbd_file_imu_frame_get_rotation_rate_z(self.ptr)

    def get_magnetic_field_x(self) -> float:
        return lib.rgbd_file_imu_frame_get_magnetic_field_x(self.ptr)

    def get_magnetic_field_y(self) -> float:
        return lib.rgbd_file_imu_frame_get_magnetic_field_y(self.ptr)

    def get_magnetic_field_z(self) -> float:
        return lib.rgbd_file_imu_frame_get_magnetic_field_z(self.ptr)

    def get_gravity_x(self) -> float:
        return lib.rgbd_file_imu_frame_get_gravity_x(self.ptr)

    def get_gravity_y(self) -> float:
        return lib.rgbd_file_imu_frame_get_gravity_y(self.ptr)

    def get_gravity_z(self) -> float:
        return lib.rgbd_file_imu_frame_get_gravity_z(self.ptr)


class NativeFileTRSFrame:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_file_trs_frame_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_time_point_us(self) -> int:
        return lib.rgbd_file_trs_frame_get_time_point_us(self.ptr)

    def get_translation_x(self) -> float:
        return lib.rgbd_file_trs_frame_get_translation_x(self.ptr)

    def get_translation_y(self) -> float:
        return lib.rgbd_file_trs_frame_get_translation_y(self.ptr)

    def get_translation_z(self) -> float:
        return lib.rgbd_file_trs_frame_get_translation_z(self.ptr)

    def get_rotation_w(self) -> float:
        return lib.rgbd_file_trs_frame_get_rotation_w(self.ptr)

    def get_rotation_x(self) -> float:
        return lib.rgbd_file_trs_frame_get_rotation_x(self.ptr)

    def get_rotation_y(self) -> float:
        return lib.rgbd_file_trs_frame_get_rotation_y(self.ptr)

    def get_rotation_z(self) -> float:
        return lib.rgbd_file_trs_frame_get_rotation_z(self.ptr)

    def get_scale_x(self) -> float:
        return lib.rgbd_file_trs_frame_get_scale_x(self.ptr)

    def get_scale_y(self) -> float:
        return lib.rgbd_file_trs_frame_get_scale_y(self.ptr)

    def get_scale_z(self) -> float:
        return lib.rgbd_file_trs_frame_get_scale_z(self.ptr)


class NativeFile:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_file_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_info(self) -> NativeFileInfo:
        return NativeFileInfo(lib.rgbd_file_get_info(self.ptr), False)

    def get_tracks(self) -> NativeFileTracks:
        return NativeFileTracks(lib.rgbd_file_get_tracks(self.ptr), False)

    def get_attachments(self) -> NativeFileAttachments:
        return NativeFileAttachments(lib.rgbd_file_get_attachments(self.ptr), False)

    def get_video_frame_count(self) -> int:
        return lib.rgbd_file_get_video_frame_count(self.ptr)

    def get_video_frame(self, index: int) -> NativeFileVideoFrame:
        return NativeFileVideoFrame(lib.rgbd_file_get_video_frame(self.ptr, index), False)

    def get_audio_frame_count(self) -> int:
        return lib.rgbd_file_get_audio_frame_count(self.ptr)

    def get_audio_frame(self, index: int) -> NativeFileAudioFrame:
        return NativeFileAudioFrame(lib.rgbd_file_get_audio_frame(self.ptr, index), False)

    def get_imu_frame_count(self) -> int:
        return lib.rgbd_file_get_imu_frame_count(self.ptr)

    def get_imu_frame(self, index: int) -> NativeFileIMUFrame:
        return NativeFileIMUFrame(lib.rgbd_file_get_imu_frame(self.ptr, index), False)

    def get_trs_frame_count(self) -> int:
        return lib.rgbd_file_get_trs_frame_count(self.ptr)

    def get_trs_frame(self, index: int) -> NativeFileTRSFrame:
        return NativeFileTRSFrame(lib.rgbd_file_get_trs_frame(self.ptr, index), False)

    def has_direction_table(self) -> bool:
        return lib.rgbd_file_has_direction_table(self.ptr)

    def get_direction_table(self) -> NativeDirectionTable:
        return NativeDirectionTable(lib.rgbd_file_get_direction_table(self.ptr), False)


class FileInfo:
    def __init__(self, duration_us: float):
        self.duration_us = duration_us

    @classmethod
    def from_native(cls, native_file_info: NativeFileInfo):
        duration_us = native_file_info.get_duration_us()
        return FileInfo(duration_us)


class FileVideoTrack:
    def __init__(self, track_number: int, width: int, height: int):
        self.track_number = track_number
        self.width = width
        self.height = height

    @classmethod
    def from_native(cls, native_file_video_track: NativeFileVideoTrack):
        track_number = native_file_video_track.get_track_number()
        width = native_file_video_track.get_width()
        height = native_file_video_track.get_height()
        return FileVideoTrack(track_number, width, height)


class FileColorVideoTrack(FileVideoTrack):
    def __init__(self, track_number: int, width: int, height: int, codec: ColorCodecType):
        super().__init__(track_number, width, height)
        self.codec = codec

    @classmethod
    def from_native(cls, native_file_video_track: NativeFileVideoTrack):
        if not isinstance(native_file_video_track, NativeFileColorVideoTrack):
            raise Exception("Found a non-NativeFileColorVideoTrack in FileColorVideoTrack.from_native")
        track_number = native_file_video_track.get_track_number()
        width = native_file_video_track.get_width()
        height = native_file_video_track.get_height()
        codec = native_file_video_track.get_codec()
        return FileColorVideoTrack(track_number, width, height, codec)


class FileDepthVideoTrack(FileVideoTrack):
    def __init__(self, track_number: int, width: int, height: int, codec: DepthCodecType, depth_unit: float):
        super().__init__(track_number, width, height)
        self.codec = codec
        self.depth_unit = depth_unit

    @classmethod
    def from_native(cls, native_file_video_track: NativeFileVideoTrack):
        if not isinstance(native_file_video_track, NativeFileDepthVideoTrack):
            raise Exception("Found a non-NativeFileDepthVideoTrack in FileDepthVideoTrack.from_native")
        track_number = native_file_video_track.get_track_number()
        width = native_file_video_track.get_width()
        height = native_file_video_track.get_height()
        codec = native_file_video_track.get_codec()
        depth_unit = native_file_video_track.get_depth_unit()
        return FileDepthVideoTrack(track_number, width, height, codec, depth_unit)

    # @classmethod
    # def from_native(cls, native_file_depth_video_track: NativeFileDepthVideoTrack):
    #     track_number = native_file_depth_video_track.get_track_number()
    #     width = native_file_depth_video_track.get_width()
    #     height = native_file_depth_video_track.get_height()
    #     codec = native_file_depth_video_track.get_codec()
    #     depth_unit = native_file_depth_video_track.get_depth_unit()
    #     return FileDepthVideoTrack(track_number, width, height, codec, depth_unit)


class FileAttachments:
    def __init__(self, camera_calibration: CameraCalibration):
        self.camera_calibration = camera_calibration

    @classmethod
    def from_native(cls, native_file_attachments: NativeFileAttachments):
        with native_file_attachments.get_camera_calibration() as native_camera_calibration:
            camera_calibration = CameraCalibration.from_native(native_camera_calibration)
        return FileAttachments(camera_calibration)


class FileTracks:
    def __init__(self, color_track: FileColorVideoTrack, depth_track: FileVideoTrack):
        self.color_track = color_track
        self.depth_track = depth_track

    @classmethod
    def from_native(cls, native_file_tracks: NativeFileTracks):
        with native_file_tracks.get_color_track() as native_color_track:
            color_track = FileColorVideoTrack.from_native(native_color_track)
        with native_file_tracks.get_depth_track() as native_depth_track:
            depth_track = FileDepthVideoTrack.from_native(native_depth_track)
        return FileTracks(color_track, depth_track)


class FileVideoFrame:
    def __init__(self, time_point_us: int, keyframe: bool,
                 color_bytes: np.ndarray, depth_bytes: np.ndarray):
        self.time_point_us = time_point_us
        self.keyframe = keyframe
        self.color_bytes = color_bytes
        self.depth_bytes = depth_bytes

    @classmethod
    def from_native(cls, native_file_video_frame: NativeFileVideoFrame):
        time_point_us = native_file_video_frame.get_time_point_us()
        keyframe = native_file_video_frame.get_keyframe()
        with native_file_video_frame.get_color_bytes() as color_bytes:
            color_bytes = color_bytes.to_np_array()
        with native_file_video_frame.get_depth_bytes() as depth_bytes:
            depth_bytes = depth_bytes.to_np_array()
        return FileVideoFrame(time_point_us, keyframe,
                              color_bytes, depth_bytes)

    def to_native(self):
        ptr = lib.rgbd_file_video_frame_ctor(self.time_point_us,
                                             self.keyframe,
                                             cast_np_array_to_pointer(self.color_bytes),
                                             len(self.color_bytes),
                                             cast_np_array_to_pointer(self.depth_bytes),
                                             len(self.depth_bytes))
        return NativeFileVideoFrame(ptr, True)


class FileAudioFrame:
    def __init__(self, time_point_us: int, bytes: np.ndarray):
        self.time_point_us = time_point_us
        self.bytes = bytes

    @classmethod
    def from_native(cls, native_file_audio_frame: NativeFileAudioFrame):
        time_point_us = native_file_audio_frame.get_time_point_us()
        with native_file_audio_frame.get_bytes() as audio_bytes:
            bytes = audio_bytes.to_np_array()
        return FileAudioFrame(time_point_us, bytes)

    def to_native(self):
        ptr = lib.rgbd_file_audio_frame_ctor(self.time_point_us,
                                             cast_np_array_to_pointer(self.bytes),
                                             len(self.bytes))
        return NativeFileAudioFrame(ptr, True)


class FileIMUFrame:
    def __init__(self, time_point_us: int,
                 acceleration: glm.vec3, rotation_rate: glm.vec3,
                 magnetic_field: glm.vec3, gravity: glm.vec3):
        self.time_point_us = time_point_us
        self.acceleration = acceleration
        self.rotation_rate = rotation_rate
        self.magnetic_field = magnetic_field
        self.gravity = gravity

    @classmethod
    def from_native(cls, native_file_imu_frame: NativeFileIMUFrame):
        time_point_us = native_file_imu_frame.get_time_point_us()

        acceleration_x = native_file_imu_frame.get_acceleration_x()
        acceleration_y = native_file_imu_frame.get_acceleration_y()
        acceleration_z = native_file_imu_frame.get_acceleration_z()
        acceleration = glm.vec3(acceleration_x, acceleration_y, acceleration_z)

        rotation_rate_x = native_file_imu_frame.get_rotation_rate_x()
        rotation_rate_y = native_file_imu_frame.get_rotation_rate_y()
        rotation_rate_z = native_file_imu_frame.get_rotation_rate_z()
        rotation_rate = glm.vec3(rotation_rate_x, rotation_rate_y, rotation_rate_z)

        magnetic_field_x = native_file_imu_frame.get_magnetic_field_x()
        magnetic_field_y = native_file_imu_frame.get_magnetic_field_y()
        magnetic_field_z = native_file_imu_frame.get_magnetic_field_z()
        magnetic_field = glm.vec3(magnetic_field_x, magnetic_field_y, magnetic_field_z)

        gravity_x = native_file_imu_frame.get_gravity_x()
        gravity_y = native_file_imu_frame.get_gravity_y()
        gravity_z = native_file_imu_frame.get_gravity_z()
        gravity = glm.vec3(gravity_x, gravity_y, gravity_z)

        return FileIMUFrame(time_point_us, acceleration, rotation_rate, magnetic_field, gravity)

    def to_native(self):
        ptr = lib.rgbd_file_imu_frame_ctor(self.time_point_us,
                                           self.acceleration.x,
                                           self.acceleration.y,
                                           self.acceleration.z,
                                           self.rotation_rate.x,
                                           self.rotation_rate.y,
                                           self.rotation_rate.z,
                                           self.magnetic_field.x,
                                           self.magnetic_field.y,
                                           self.magnetic_field.z,
                                           self.gravity.x,
                                           self.gravity.y,
                                           self.gravity.z)
        return NativeFileIMUFrame(ptr, True)


class FileTRSFrame:
    def __init__(self, time_point_us: int, translation: glm.vec3, rotation: glm.quat, scale: glm.vec3):
        self.time_point_us = time_point_us
        self.translation = translation
        self.rotation = rotation
        self.scale = scale

    @classmethod
    def from_native(cls, native_file_trs_frame: NativeFileTRSFrame):
        time_point_us = native_file_trs_frame.get_time_point_us()

        translation_x = native_file_trs_frame.get_translation_x()
        translation_y = native_file_trs_frame.get_translation_y()
        translation_z = native_file_trs_frame.get_translation_z()
        translation = glm.vec3(translation_x, translation_y, translation_z)

        rotation_w = native_file_trs_frame.get_rotation_w()
        rotation_x = native_file_trs_frame.get_rotation_x()
        rotation_y = native_file_trs_frame.get_rotation_y()
        rotation_z = native_file_trs_frame.get_rotation_z()
        rotation = glm.quat(rotation_w, rotation_x, rotation_y, rotation_z)

        scale_x = native_file_trs_frame.get_scale_x()
        scale_y = native_file_trs_frame.get_scale_y()
        scale_z = native_file_trs_frame.get_scale_z()
        scale = glm.vec3(scale_x, scale_y, scale_z)
        return FileTRSFrame(time_point_us, translation, rotation, scale)

    def to_native(self):
        ptr = lib.rgbd_file_trs_frame_ctor(self.time_point_us,
                                           self.translation.x,
                                           self.translation.y,
                                           self.translation.z,
                                           self.rotation.w,
                                           self.rotation.x,
                                           self.rotation.y,
                                           self.rotation.z,
                                           self.scale.x,
                                           self.scale.y,
                                           self.scale.z)
        return NativeFileTRSFrame(ptr, True)


class File:
    def __init__(self, info: FileInfo, tracks: FileTracks, attachments: FileAttachments,
                 video_frames: list[FileVideoFrame], audio_frames: list[FileAudioFrame],
                 imu_frames: list[FileIMUFrame], trs_frames: list[FileTRSFrame],
                 direction_table: Optional[DirectionTable]):
        self.info = info
        self.tracks = tracks
        self.attachments = attachments
        self.video_frames = video_frames
        self.audio_frames = audio_frames
        self.imu_frames = imu_frames
        self.trs_frames = trs_frames
        self.direction_table = direction_table

    @classmethod
    def from_native(cls, native_file: NativeFile):
        with native_file.get_info() as native_info:
            info = FileInfo.from_native(native_info)
        with native_file.get_tracks() as native_tracks:
            tracks = FileTracks.from_native(native_tracks)
        with native_file.get_attachments() as native_attachments:
            attachments = FileAttachments.from_native(native_attachments)

        video_frames = []
        video_frame_count = native_file.get_video_frame_count()
        for index in range(video_frame_count):
            with native_file.get_video_frame(index) as native_file_video_frame:
                video_frames.append(FileVideoFrame.from_native(native_file_video_frame))

        audio_frames = []
        audio_frame_count = native_file.get_audio_frame_count()
        for index in range(audio_frame_count):
            with native_file.get_audio_frame(index) as native_file_audio_frame:
                audio_frames.append(FileAudioFrame.from_native(native_file_audio_frame))

        imu_frames = []
        imu_frame_count = native_file.get_imu_frame_count()
        for index in range(imu_frame_count):
            with native_file.get_imu_frame(index) as native_file_imu_frame:
                imu_frames.append(FileIMUFrame.from_native(native_file_imu_frame))

        trs_frames = []
        trs_frame_count = native_file.get_trs_frame_count()
        for index in range(trs_frame_count):
            with native_file.get_trs_frame(index) as native_file_trs_frame:
                trs_frames.append(FileTRSFrame.from_native(native_file_trs_frame))

        if native_file.has_direction_table():
            direction_table = DirectionTable.from_native(native_file.get_direction_table())
        else:
            direction_table = None
        return File(info, tracks, attachments, video_frames, audio_frames, imu_frames, trs_frames, direction_table)


def get_calibration_directions(native_file: NativeFile) -> np.ndarray:
    # Get directions array from the native_camera_calibration.
    # native_camera_calibration should be GC'ed here while directions will be needed.
    directions = []
    with native_file.get_attachments() as native_attachments:
        with native_attachments.get_camera_calibration() as native_camera_calibration:
            depth_width = native_camera_calibration.get_depth_width()
            depth_height = native_camera_calibration.get_depth_height()
            for row in range(depth_height):
                v = row / depth_height
                for col in range(depth_width):
                    u = col / depth_width
                    with native_camera_calibration.get_direction(u, v) as native_direction:
                        directions.append(native_direction.to_np_array())

    return np.reshape(directions, (depth_height, depth_width, 3))
