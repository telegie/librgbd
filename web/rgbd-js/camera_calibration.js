import { NativeFloatArray } from './capi_containers.js';

export const RGBD_CAMERA_DEVICE_TYPE_AZURE_KINECT = 0;
export const RGBD_CAMERA_DEVICE_TYPE_IOS = 1;
export const RGBD_CAMERA_DEVICE_TYPE_UNDISTORTED = 2;

export class CameraCalibration {
  constructor(nativeCameraCalibration) {
    this.wasmModule = nativeCameraCalibration.wasmModule;
    this.cameraDeviceType = nativeCameraCalibration.getCameraDeviceType();
    this.colorWidth = nativeCameraCalibration.getColorWidth();
    this.colorHeight = nativeCameraCalibration.getColorHeight();
    this.depthWidth = nativeCameraCalibration.getDepthWidth();
    this.depthHeight = nativeCameraCalibration.getDepthHeight();
  }

  static create(nativeCameraCalibration) {
    const cameraDeviceType = nativeCameraCalibration.getCameraDeviceType();
    switch(cameraDeviceType) {
      case RGBD_CAMERA_DEVICE_TYPE_AZURE_KINECT:
        return new KinectCameraCalibration(nativeCameraCalibration);
      case RGBD_CAMERA_DEVICE_TYPE_IOS:
        return new IosCameraCalibration(nativeCameraCalibration);
      case RGBD_CAMERA_DEVICE_TYPE_UNDISTORTED:
        return new UndistortedCameraCalibration(nativeCameraCalibration);
    }
    throw Error('Failed to infer device type in CameraCalibration.create');
  }

  createNativeInstance() {
    throw Error('CameraCalibration.createNativeInstance not implemented.');
  }
}

export class KinectCameraCalibration extends CameraCalibration {
  constructor(nativeKinectCameraCalibration) {
    super(nativeKinectCameraCalibration);
    this.colorWidth = nativeKinectCameraCalibration.getColorWidth();
    this.colorHeight = nativeKinectCameraCalibration.getColorHeight();
    this.depthWidth = nativeKinectCameraCalibration.getDepthWidth();
    this.depthHeight = nativeKinectCameraCalibration.getDepthHeight();
    this.resolutionWidth = nativeKinectCameraCalibration.getResolutionWidth();
    this.resolutionHeight = nativeKinectCameraCalibration.getResolutionHeight();
    this.cx = nativeKinectCameraCalibration.getCx();
    this.cy = nativeKinectCameraCalibration.getCy();
    this.fx = nativeKinectCameraCalibration.getFx();
    this.fy = nativeKinectCameraCalibration.getFy();
    this.k1 = nativeKinectCameraCalibration.getK1();
    this.k2 = nativeKinectCameraCalibration.getK2();
    this.k3 = nativeKinectCameraCalibration.getK3();
    this.k4 = nativeKinectCameraCalibration.getK4();
    this.k5 = nativeKinectCameraCalibration.getK5();
    this.k6 = nativeKinectCameraCalibration.getK6();
    this.codx = nativeKinectCameraCalibration.getCodx();
    this.cody = nativeKinectCameraCalibration.getCody();
    this.p1 = nativeKinectCameraCalibration.getP1();
    this.p2 = nativeKinectCameraCalibration.getP2();
    this.maxRadiusForProjection = nativeKinectCameraCalibration.getMaxRadiusForProjection();
  }

  createNativeInstance() {
    const nativePtr = this.wasmModule.ccall('rgbd_kinect_camera_calibration_ctor',
                                            'number',
                                            ['number', 'number', 'number', 'number',
                                             'number', 'number',
                                             'number', 'number', 'number', 'number',
                                             'number', 'number', 'number', 'number', 'number', 'number',
                                             'number', 'number', 'number', 'number',
                                             'number'],
                                            [this.colorWidth, this.colorHeight, this.depthWidth, this.depthHeight,
                                             this.resolutionWidth, this.resolutionHeight,
                                             this.cx, this.cy, this.fx, this.fy,
                                             this.k1, this.k2, this.k3, this.k4, this.k5, this.k6,
                                             this.codx, this.cody, this.p1, this.p2,
                                             this.maxRadiusForProjection]);
    return new NativeKinectCameraCalibration(this.wasmModule, nativePtr, true);
  }
}

export class IosCameraCalibration extends CameraCalibration {
  constructor(nativeIosCameraCalibration) {
    super(nativeIosCameraCalibration);
    this.fx = nativeIosCameraCalibration.getFx();
    this.fy = nativeIosCameraCalibration.getFy();
    this.ox = nativeIosCameraCalibration.getOx();
    this.oy = nativeIosCameraCalibration.getOy();
    this.referenceDimensionWidth = nativeIosCameraCalibration.getReferenceDimensionWidth();
    this.referenceDimensionHeight = nativeIosCameraCalibration.getReferenceDimensionHeight();
    this.lensDistortionCenterX = nativeIosCameraCalibration.getLensDistortionCenterX();
    this.lensDistortionCenterY = nativeIosCameraCalibration.getLensDistortionCenterY();
    this.lensDistortionLookupTable = nativeIosCameraCalibration.getLensDistortionLookupTable();
  }

  createNativeInstance() {
    const lookupTablePtr = this.wasmModule._malloc(this.lensDistortionLookupTable.byteLength);
    // Have to do >> 2 to the pointer since the set() function interprets its second parameter
    // as an index, not a pointer.
    // https://github.com/emscripten-core/emscripten/issues/4003
    this.wasmModule.HEAPF32.set(this.lensDistortionLookupTable, lookupTablePtr >> 2);
    const nativePtr = this.wasmModule.ccall('rgbd_ios_camera_calibration_ctor',
                                            'number',
                                            ['number', 'number', 'number', 'number',
                                             'number', 'number', 'number', 'number',
                                             'number', 'number',
                                             'number', 'number',
                                             'number',
                                             'number'],
                                            [this.colorWidth, this.colorHeight, this.depthWidth, this.depthHeight,
                                             this.fx, this.fy, this.ox, this.oy,
                                             this.referenceDimensionWidth, this.referenceDimensionHeight,
                                             this.lensDistortionCenterX, this.lensDistortionCenterY,
                                             lookupTablePtr,
                                             this.lensDistortionLookupTable.length]);
    this.wasmModule._free(lookupTablePtr);

    return new NativeIosCameraCalibration(this.wasmModule, nativePtr, true);
  }
}

export class UndistortedCameraCalibration extends CameraCalibration {
  constructor(nativeUndistortedCameraCalibration) {
    super(nativeUndistortedCameraCalibration);
    this.fx = nativeUndistortedCameraCalibration.getFx();
    this.fy = nativeUndistortedCameraCalibration.getFy();
    this.cx = nativeUndistortedCameraCalibration.getCx();
    this.cy = nativeUndistortedCameraCalibration.getCy();
  }

  createNativeInstance() {
    const nativePtr = this.wasmModule.ccall('rgbd_undistorted_camera_calibration_ctor',
                                            'number',
                                            ['number', 'number', 'number', 'number',
                                             'number', 'number',
                                             'number', 'number', 'number', 'number',
                                             'number', 'number', 'number', 'number', 'number', 'number',
                                             'number', 'number', 'number', 'number',
                                             'number'],
                                            [this.colorWidth, this.colorHeight, this.depthWidth, this.depthHeight,
                                             this.fx, this.fy, this.cx, this.cy]);
    return new NativeUndistortedCameraCalibration(this.wasmModule, nativePtr, true);
  }
}

export class NativeCameraCalibration {
  // Do not directly use this init.
  // An instance created directly using this won't contain the class type
  // information of the C++ instance.
  // Use create() instead.
  constructor(wasmModule, ptr, owner) {
      this.wasmModule = wasmModule;
      this.ptr = ptr;
      this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_camera_calibration_dtor', null, ['number'], [this.ptr]);
  }

  // Use this instead of the raw init to create an instance
  // containing the class information matching its corresponding C++ instance.
  static create(wasmModule, ptr, owner) {
    const cameraDeviceType = wasmModule.ccall('rgbd_camera_calibration_get_camera_device_type', 'number', ['number'], [ptr]);

    switch (cameraDeviceType) {
      case RGBD_CAMERA_DEVICE_TYPE_AZURE_KINECT:
        return new NativeKinectCameraCalibration(wasmModule, ptr, owner)
      case RGBD_CAMERA_DEVICE_TYPE_IOS:
        return new NativeIosCameraCalibration(wasmModule, ptr, owner)
      case RGBD_CAMERA_DEVICE_TYPE_UNDISTORTED:
        return new NativeUndistortedCameraCalibration(wasmModule, ptr, owner)
    }
    throw Error('not supported camera device type found')
  }

  getCameraDeviceType() {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_camera_device_type', 'number', ['number'], [this.ptr]);
  }

  getColorWidth() {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_color_width', 'number', ['number'], [this.ptr]);
  }

  getColorHeight() {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_color_height', 'number', ['number'], [this.ptr]);
  }

  getDepthWidth() {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_depth_width', 'number', ['number'], [this.ptr]);
  }

  getDepthHeight() {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_depth_height', 'number', ['number'], [this.ptr]);
  }
}

export class NativeKinectCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule, ptr, owner) {
    super(wasmModule, ptr, owner);
  }

  getResolutionWidth() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_resolution_width', 'number', ['number'], [this.ptr]);
  }

  getResolutionHeight() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_resolution_height', 'number', ['number'], [this.ptr]);
  }

  getCx() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_cx', 'number', ['number'], [this.ptr]);
  }

  getCy() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_cy', 'number', ['number'], [this.ptr]);
  }

  getFx() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_fx', 'number', ['number'], [this.ptr]);
  }

  getFy() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_fy', 'number', ['number'], [this.ptr]);
  }

  getK1() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k1', 'number', ['number'], [this.ptr]);
  }

  getK2() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k2', 'number', ['number'], [this.ptr]);
  }

  getK3() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k3', 'number', ['number'], [this.ptr]);
  }

  getK4() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k4', 'number', ['number'], [this.ptr]);
  }

  getK5() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k5', 'number', ['number'], [this.ptr]);
  }

  getK6() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k6', 'number', ['number'], [this.ptr]);
  }

  getCodx() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_codx', 'number', ['number'], [this.ptr]);
  }

  getCody() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_cody', 'number', ['number'], [this.ptr]);
  }

  getP1() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_p1', 'number', ['number'], [this.ptr]);
  }

  getP2() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_p2', 'number', ['number'], [this.ptr]);
  }

  getMaxRadiusForProjection() {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_max_radius_for_projection', 'number', ['number'], [this.ptr]);
  }
}

export class NativeIosCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule, ptr, owner) {
    super(wasmModule, ptr, owner);
  }

  getFx() {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_fx', 'number', ['number'], [this.ptr]);
  }

  getFy() {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_fy', 'number', ['number'], [this.ptr]);
  }

  getOx() {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_ox', 'number', ['number'], [this.ptr]);
  }

  getOy() {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_oy', 'number', ['number'], [this.ptr]);
  }

  getReferenceDimensionWidth() {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_reference_dimension_width', 'number', ['number'], [this.ptr]);
  }

  getReferenceDimensionHeight() {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_reference_dimension_height', 'number', ['number'], [this.ptr]);
  }

  getLensDistortionCenterX() {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_lens_distortion_center_x', 'number', ['number'], [this.ptr]);
  }

  getLensDistortionCenterY() {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_lens_distortion_center_y', 'number', ['number'], [this.ptr]);
  }

  getLensDistortionLookupTable() {
    const nativeFloatArrayPtr = this.wasmModule.ccall('rgbd_ios_camera_calibration_get_lens_distortion_lookup_table', 'number', ['number'], [this.ptr]);
    const nativeFloatArray = new NativeFloatArray(this.wasmModule, nativeFloatArrayPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    return floatArray;
  }
}

export class NativeUndistortedCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule, ptr, owner) {
    super(wasmModule, ptr, owner);
  }

  getFx() {
    return this.wasmModule.ccall('rgbd_undistorted_camera_calibration_get_fx', 'number', ['number'], [this.ptr]);
  }

  getFy() {
    return this.wasmModule.ccall('rgbd_undistorted_camera_calibration_get_fy', 'number', ['number'], [this.ptr]);
  }

  getCx() {
    return this.wasmModule.ccall('rgbd_undistorted_camera_calibration_get_cx', 'number', ['number'], [this.ptr]);
  }

  getCy() {
    return this.wasmModule.ccall('rgbd_undistorted_camera_calibration_get_cy', 'number', ['number'], [this.ptr]);
  }
}
