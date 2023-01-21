from enum import IntEnum
from typing import cast
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
    def __init__(self, native_camera_calibration: NativeCameraCalibration):
        self.camera_device_type = native_camera_calibration.get_camera_device_type()
        self.color_width = native_camera_calibration.get_color_width()
        self.color_height = native_camera_calibration.get_color_height()
        self.depth_width = native_camera_calibration.get_depth_width()
        self.depth_height = native_camera_calibration.get_depth_height()

    @classmethod
    def from_native(cls, native_camera_calibration: NativeCameraCalibration):
        camera_device_type = native_camera_calibration.get_camera_device_type()
        if camera_device_type == CameraDeviceType.AZURE_KINECT:
            native_kinect_camera_calibration = cast(NativeKinectCameraCalibration, native_camera_calibration)
            return KinectCameraCalibration(native_kinect_camera_calibration)
        if camera_device_type == CameraDeviceType.IOS:
            native_ios_camera_calibration = cast(NativeIosCameraCalibration, native_camera_calibration)
            return IosCameraCalibration(native_ios_camera_calibration)
        if camera_device_type == CameraDeviceType.UNDISTORTED:
            native_undistorted_camera_calibration = cast(NativeUndistortedCameraCalibration, native_camera_calibration)
            return UndistortedCameraCalibration(native_undistorted_camera_calibration)

        raise RuntimeError("Failed to infer device type in CameraCalibration.create")

    def to_native(self) -> NativeCameraCalibration:
        raise RuntimeError("CameraCalibration.create_native_instance not implemented.")


class KinectCameraCalibration(CameraCalibration):
    def __init__(self, native_kinect_camera_calibration: NativeKinectCameraCalibration):
        super().__init__(native_kinect_camera_calibration)
        self.resolution_width = native_kinect_camera_calibration.get_resolution_width()
        self.resolution_height = native_kinect_camera_calibration.get_resolution_height()
        self.cx = native_kinect_camera_calibration.get_cx()
        self.cy = native_kinect_camera_calibration.get_cy()
        self.fx = native_kinect_camera_calibration.get_fx()
        self.fy = native_kinect_camera_calibration.get_fy()
        self.k1 = native_kinect_camera_calibration.get_k1()
        self.k2 = native_kinect_camera_calibration.get_k2()
        self.k3 = native_kinect_camera_calibration.get_k3()
        self.k4 = native_kinect_camera_calibration.get_k4()
        self.k5 = native_kinect_camera_calibration.get_k5()
        self.k6 = native_kinect_camera_calibration.get_k6()
        self.codx = native_kinect_camera_calibration.get_codx()
        self.cody = native_kinect_camera_calibration.get_cody()
        self.p1 = native_kinect_camera_calibration.get_p1()
        self.p2 = native_kinect_camera_calibration.get_p2()
        self.max_radius_for_projection = native_kinect_camera_calibration.get_max_radius_for_projection()

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
    def __init__(self, native_ios_camera_calibration: NativeIosCameraCalibration):
        super().__init__(native_ios_camera_calibration)
        self.fx = native_ios_camera_calibration.get_fx()
        self.fy = native_ios_camera_calibration.get_fy()
        self.ox = native_ios_camera_calibration.get_ox()
        self.oy = native_ios_camera_calibration.get_oy()
        self.reference_dimension_width = native_ios_camera_calibration.get_reference_dimension_width()
        self.reference_dimension_height = native_ios_camera_calibration.get_reference_dimension_height()
        self.lens_distortion_center_x = native_ios_camera_calibration.get_lens_distortion_center_x()
        self.lens_distortion_center_y = native_ios_camera_calibration.get_lens_distortion_center_y()
        with native_ios_camera_calibration.get_lens_distortion_lookup_table() as lens_distortion_lookup_table:
            self.lens_distortion_lookup_table = lens_distortion_lookup_table.to_np_array()
        with native_ios_camera_calibration.get_inverse_lens_distortion_lookup_table() as lookup_table:
            self.inverse_lens_distortion_lookup_table = lookup_table.to_np_array()

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
    def __init__(self, native_undistorted_camera_calibration: NativeUndistortedCameraCalibration):
        super().__init__(native_undistorted_camera_calibration)
        self.fx = native_undistorted_camera_calibration.get_fx()
        self.fy = native_undistorted_camera_calibration.get_fy()
        self.cx = native_undistorted_camera_calibration.get_cx()
        self.cy = native_undistorted_camera_calibration.get_cy()

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
