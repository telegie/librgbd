from enum import IntEnum
from typing import cast
import numpy as np
from ._librgbd_ffi import lib
from .capi_containers import NativeFloatArray
from .utils import cast_np_array_to_pointer


class CameraDeviceType(IntEnum):
    AZURE_KINECT = lib.RGBD_CAMERA_DEVICE_TYPE_AZURE_KINECT
    IOS = lib.RGBD_CAMERA_DEVICE_TYPE_IOS
    UNDISTORTED = lib.RGBD_CAMERA_DEVICE_TYPE_UNDISTORTED


class NativeCameraCalibration:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_camera_calibration_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    @staticmethod
    def create(ptr, owner: bool):
        camera_device_type = CameraDeviceType(lib.rgbd_camera_calibration_get_camera_device_type(ptr))
        if camera_device_type == CameraDeviceType.AZURE_KINECT:
            return NativeKinectCameraCalibration(ptr, owner)
        if camera_device_type == CameraDeviceType.IOS:
            return NativeIosCameraCalibration(ptr, owner)
        if camera_device_type == CameraDeviceType.UNDISTORTED:
            return NativeUndistortedCameraCalibration(ptr, owner)

        raise RuntimeError("Not supported camera device type found.")

    def get_camera_device_type(self) -> CameraDeviceType:
        return CameraDeviceType(lib.rgbd_camera_calibration_get_camera_device_type(self.ptr))

    def get_color_width(self) -> int:
        return lib.rgbd_camera_calibration_get_color_width(self.ptr)

    def get_color_height(self) -> int:
        return lib.rgbd_camera_calibration_get_color_height(self.ptr)

    def get_depth_width(self) -> int:
        return lib.rgbd_camera_calibration_get_depth_width(self.ptr)

    def get_depth_height(self) -> int:
        return lib.rgbd_camera_calibration_get_depth_height(self.ptr)

    def get_direction(self, uv_u: float, uv_v: float) -> NativeFloatArray:
        return NativeFloatArray(lib.rgbd_camera_calibration_get_direction(self.ptr, uv_u, uv_v))


class NativeKinectCameraCalibration(NativeCameraCalibration):
    def __init__(self, ptr, owner: bool):
        super().__init__(ptr, owner)

    def get_resolution_width(self):
        return lib.rgbd_kinect_camera_calibration_get_resolution_width(self.ptr)

    def get_resolution_height(self):
        return lib.rgbd_kinect_camera_calibration_get_resolution_height(self.ptr)

    def get_cx(self):
        return lib.rgbd_kinect_camera_calibration_get_cx(self.ptr)

    def get_cy(self):
        return lib.rgbd_kinect_camera_calibration_get_cy(self.ptr)

    def get_fx(self):
        return lib.rgbd_kinect_camera_calibration_get_fx(self.ptr)

    def get_fy(self):
        return lib.rgbd_kinect_camera_calibration_get_fy(self.ptr)

    def get_k1(self):
        return lib.rgbd_kinect_camera_calibration_get_k1(self.ptr)

    def get_k2(self):
        return lib.rgbd_kinect_camera_calibration_get_k2(self.ptr)

    def get_k3(self):
        return lib.rgbd_kinect_camera_calibration_get_k3(self.ptr)

    def get_k4(self):
        return lib.rgbd_kinect_camera_calibration_get_k4(self.ptr)

    def get_k5(self):
        return lib.rgbd_kinect_camera_calibration_get_k5(self.ptr)

    def get_k6(self):
        return lib.rgbd_kinect_camera_calibration_get_k6(self.ptr)

    def get_codx(self):
        return lib.rgbd_kinect_camera_calibration_get_codx(self.ptr)

    def get_cody(self):
        return lib.rgbd_kinect_camera_calibration_get_cody(self.ptr)

    def get_p1(self):
        return lib.rgbd_kinect_camera_calibration_get_p1(self.ptr)

    def get_p2(self):
        return lib.rgbd_kinect_camera_calibration_get_p2(self.ptr)

    def get_max_radius_for_projection(self):
        return lib.rgbd_kinect_camera_calibration_get_max_radius_for_projection(self.ptr)


class NativeIosCameraCalibration(NativeCameraCalibration):
    def __init__(self, ptr, owner: bool):
        super().__init__(ptr, owner)

    def get_fx(self):
        return lib.rgbd_ios_camera_calibration_get_fx(self.ptr)

    def get_fy(self):
        return lib.rgbd_ios_camera_calibration_get_fy(self.ptr)

    def get_ox(self):
        return lib.rgbd_ios_camera_calibration_get_ox(self.ptr)

    def get_oy(self):
        return lib.rgbd_ios_camera_calibration_get_oy(self.ptr)

    def get_reference_dimension_width(self):
        return lib.rgbd_ios_camera_calibration_get_reference_dimension_width(self.ptr)

    def get_reference_dimension_height(self):
        return lib.rgbd_ios_camera_calibration_get_reference_dimension_height(self.ptr)

    def get_lens_distortion_center_x(self):
        return lib.rgbd_ios_camera_calibration_get_lens_distortion_center_x(self.ptr)

    def get_lens_distortion_center_y(self):
        return lib.rgbd_ios_camera_calibration_get_lens_distortion_center_y(self.ptr)

    def get_lens_distortion_lookup_table(self) -> NativeFloatArray:
        return NativeFloatArray(lib.rgbd_ios_camera_calibration_get_lens_distortion_lookup_table(self.ptr))

    def get_inverse_lens_distortion_lookup_table(self) -> NativeFloatArray:
        return NativeFloatArray(lib.rgbd_ios_camera_calibration_get_inverse_lens_distortion_lookup_table(self.ptr))


class NativeUndistortedCameraCalibration(NativeCameraCalibration):
    def __init__(self, ptr, owner: bool):
        super().__init__(ptr, owner)

    def get_fx(self):
        return lib.rgbd_undistorted_camera_calibration_get_fx(self.ptr)

    def get_fy(self):
        return lib.rgbd_undistorted_camera_calibration_get_fy(self.ptr)

    def get_cx(self):
        return lib.rgbd_undistorted_camera_calibration_get_cx(self.ptr)

    def get_cy(self):
        return lib.rgbd_undistorted_camera_calibration_get_cy(self.ptr)


class CameraCalibration:
    def __init__(self, camera_device_type: CameraDeviceType,
                 color_width: int, color_height: int,
                 depth_width: int, depth_height: int):
        self.camera_device_type = camera_device_type
        self.color_width = color_width
        self.color_height = color_height
        self.depth_width = depth_width
        self.depth_height = depth_height

    @classmethod
    def from_native(cls, native_camera_calibration: NativeCameraCalibration):
        camera_device_type = native_camera_calibration.get_camera_device_type()
        if camera_device_type == CameraDeviceType.AZURE_KINECT:
            native_kinect_camera_calibration = cast(NativeKinectCameraCalibration, native_camera_calibration)
            return KinectCameraCalibration.from_native(native_kinect_camera_calibration)
        if camera_device_type == CameraDeviceType.IOS:
            native_ios_camera_calibration = cast(NativeIosCameraCalibration, native_camera_calibration)
            return IosCameraCalibration.from_native(native_ios_camera_calibration)
        if camera_device_type == CameraDeviceType.UNDISTORTED:
            native_undistorted_camera_calibration = cast(NativeUndistortedCameraCalibration, native_camera_calibration)
            return UndistortedCameraCalibration.from_native(native_undistorted_camera_calibration)

        raise RuntimeError("Failed to infer device type in CameraCalibration.create")

    def to_native(self) -> NativeCameraCalibration:
        raise RuntimeError("CameraCalibration.create_native_instance not implemented.")


class KinectCameraCalibration(CameraCalibration):
    def __init__(self, camera_device_type: CameraDeviceType,
                 color_width: int, color_height: int,
                 depth_width: int, depth_height: int,
                 resolution_width: int, resolution_height: int,
                 cx: float, cy: float, fx: float, fy: float,
                 k1: float, k2: float, k3: float, k4: float, k5: float, k6: float,
                 codx: float, cody: float, p1: float, p2: float,
                 max_radius_for_projection: float):
        super().__init__(camera_device_type,
                         color_width, color_height,
                         depth_width, depth_height)
        self.resolution_width = resolution_width
        self.resolution_height = resolution_height
        self.cx = cx
        self.cy = cy
        self.fx = fx
        self.fy = fy
        self.k1 = k1
        self.k2 = k2
        self.k3 = k3
        self.k4 = k4
        self.k5 = k5
        self.k6 = k6
        self.codx = codx
        self.cody = cody
        self.p1 = p1
        self.p2 = p2
        self.max_radius_for_projection = max_radius_for_projection

    @classmethod
    def from_native(cls, native_kinect_camera_calibration: NativeKinectCameraCalibration):
        camera_device_type = native_kinect_camera_calibration.get_camera_device_type()
        color_width = native_kinect_camera_calibration.get_color_width()
        color_height = native_kinect_camera_calibration.get_color_height()
        depth_width = native_kinect_camera_calibration.get_depth_width()
        depth_height = native_kinect_camera_calibration.get_depth_height()
        resolution_width = native_kinect_camera_calibration.get_resolution_width()
        resolution_height = native_kinect_camera_calibration.get_resolution_height()
        cx = native_kinect_camera_calibration.get_cx()
        cy = native_kinect_camera_calibration.get_cy()
        fx = native_kinect_camera_calibration.get_fx()
        fy = native_kinect_camera_calibration.get_fy()
        k1 = native_kinect_camera_calibration.get_k1()
        k2 = native_kinect_camera_calibration.get_k2()
        k3 = native_kinect_camera_calibration.get_k3()
        k4 = native_kinect_camera_calibration.get_k4()
        k5 = native_kinect_camera_calibration.get_k5()
        k6 = native_kinect_camera_calibration.get_k6()
        codx = native_kinect_camera_calibration.get_codx()
        cody = native_kinect_camera_calibration.get_cody()
        p1 = native_kinect_camera_calibration.get_p1()
        p2 = native_kinect_camera_calibration.get_p2()
        max_radius_for_projection = native_kinect_camera_calibration.get_max_radius_for_projection()
        return KinectCameraCalibration(camera_device_type,
                                       color_width, color_height,
                                       depth_width, depth_height,
                                       resolution_width, resolution_height,
                                       cx, cy, fx, fy,
                                       k1, k2, k3, k4, k5, k6,
                                       codx, cody, p1, p2,
                                       max_radius_for_projection)

    def to_native(self) -> NativeKinectCameraCalibration:
        ptr = lib.rgbd_kinect_camera_calibration_ctor(self.color_width,
                                                      self.color_height,
                                                      self.depth_width,
                                                      self.depth_height,
                                                      self.resolution_width,
                                                      self.resolution_height,
                                                      self.cx,
                                                      self.cy,
                                                      self.fx,
                                                      self.fy,
                                                      self.k1,
                                                      self.k2,
                                                      self.k3,
                                                      self.k4,
                                                      self.k5,
                                                      self.k6,
                                                      self.codx,
                                                      self.cody,
                                                      self.p1,
                                                      self.p2,
                                                      self.max_radius_for_projection)
        return NativeKinectCameraCalibration(ptr, True)


class IosCameraCalibration(CameraCalibration):
    def __init__(self, camera_device_type: CameraDeviceType,
                 color_width: int, color_height: int,
                 depth_width: int, depth_height: int,
                 fx: float, fy: float, ox: float, oy: float,
                 reference_dimension_width: float, reference_dimension_height: float,
                 lens_distortion_center_x: float, lens_distortion_center_y: float,
                 lens_distortion_lookup_table: np.array,
                 inverse_lens_distortion_lookup_table: np.array):
        super().__init__(camera_device_type,
                         color_width, color_height,
                         depth_width, depth_height)
        self.fx = fx
        self.fy = fy
        self.ox = ox
        self.oy = oy
        self.reference_dimension_width = reference_dimension_width
        self.reference_dimension_height = reference_dimension_height
        self.lens_distortion_center_x = lens_distortion_center_x
        self.lens_distortion_center_y = lens_distortion_center_y
        self.lens_distortion_lookup_table = lens_distortion_lookup_table
        self.inverse_lens_distortion_lookup_table = inverse_lens_distortion_lookup_table

    @classmethod
    def from_native(cls, native_ios_camera_calibration: NativeIosCameraCalibration):
        camera_device_type = native_ios_camera_calibration.get_camera_device_type()
        color_width = native_ios_camera_calibration.get_color_width()
        color_height = native_ios_camera_calibration.get_color_height()
        depth_width = native_ios_camera_calibration.get_depth_width()
        depth_height = native_ios_camera_calibration.get_depth_height()
        fx = native_ios_camera_calibration.get_fx()
        fy = native_ios_camera_calibration.get_fy()
        ox = native_ios_camera_calibration.get_ox()
        oy = native_ios_camera_calibration.get_oy()
        reference_dimension_width = native_ios_camera_calibration.get_reference_dimension_width()
        reference_dimension_height = native_ios_camera_calibration.get_reference_dimension_height()
        lens_distortion_center_x = native_ios_camera_calibration.get_lens_distortion_center_x()
        lens_distortion_center_y = native_ios_camera_calibration.get_lens_distortion_center_y()
        with native_ios_camera_calibration.get_lens_distortion_lookup_table() as lens_distortion_lookup_table:
            lens_distortion_lookup_table = lens_distortion_lookup_table.to_np_array()
        with native_ios_camera_calibration.get_inverse_lens_distortion_lookup_table() as lookup_table:
            inverse_lens_distortion_lookup_table = lookup_table.to_np_array()
        return IosCameraCalibration(camera_device_type,
                                    color_width, color_height,
                                    depth_width, depth_height,
                                    fx, fy, ox, oy,
                                    reference_dimension_width, reference_dimension_height,
                                    lens_distortion_center_x, lens_distortion_center_y,
                                    lens_distortion_lookup_table, inverse_lens_distortion_lookup_table)

    def to_native(self) -> NativeIosCameraCalibration:
        ptr = lib.rgbd_ios_camera_calibration_ctor(self.color_width,
                                                   self.color_height,
                                                   self.depth_width,
                                                   self.depth_height,
                                                   self.fx,
                                                   self.fy,
                                                   self.ox,
                                                   self.oy,
                                                   self.reference_dimension_width,
                                                   self.reference_dimension_height,
                                                   self.lens_distortion_center_x,
                                                   self.lens_distortion_center_y,
                                                   cast_np_array_to_pointer(self.lens_distortion_lookup_table),
                                                   self.lens_distortion_lookup_table.size,
                                                   cast_np_array_to_pointer(self.inverse_lens_distortion_lookup_table),
                                                   self.inverse_lens_distortion_lookup_table.size)
        return NativeIosCameraCalibration(ptr, True)


class UndistortedCameraCalibration(CameraCalibration):
    def __init__(self, camera_device_type: CameraDeviceType,
                 color_width: int, color_height: int,
                 depth_width: int, depth_height: int,
                 fx: float, fy: float, cx: float, cy: float):
        super().__init__(camera_device_type,
                         color_width, color_height,
                         depth_width, depth_height)
        self.fx = fx
        self.fy = fy
        self.cx = cx
        self.cy = cy

    @classmethod
    def from_native(cls, native_undistorted_camera_calibration: NativeUndistortedCameraCalibration):
        camera_device_type = native_undistorted_camera_calibration.get_camera_device_type()
        color_width = native_undistorted_camera_calibration.get_color_width()
        color_height = native_undistorted_camera_calibration.get_color_height()
        depth_width = native_undistorted_camera_calibration.get_depth_width()
        depth_height = native_undistorted_camera_calibration.get_depth_height()
        fx = native_undistorted_camera_calibration.get_fx()
        fy = native_undistorted_camera_calibration.get_fy()
        cx = native_undistorted_camera_calibration.get_cx()
        cy = native_undistorted_camera_calibration.get_cy()
        return UndistortedCameraCalibration(camera_device_type,
                                            color_width, color_height,
                                            depth_width, depth_height,
                                            fx, fy, cx, cy)

    def to_native(self) -> NativeUndistortedCameraCalibration:
        ptr = lib.rgbd_undistorted_camera_calibration_ctor(self.color_width,
                                                           self.color_height,
                                                           self.depth_width,
                                                           self.depth_height,
                                                           self.fx,
                                                           self.fy,
                                                           self.cx,
                                                           self.cy)
        return NativeUndistortedCameraCalibration(ptr, True)


def create_native_undistorted_camera_calibration(color_width: int, color_height: int,
                                                 depth_width: int, depth_height: int,
                                                 fx: float, fy: float,
                                                 cx: float, cy: float) -> NativeUndistortedCameraCalibration:
    ptr = lib.rgbd_undistorted_camera_calibration_ctor(color_width, color_height, depth_width, depth_height,
                                                       fx, fy, cx, cy)
    return NativeUndistortedCameraCalibration(ptr, True)
