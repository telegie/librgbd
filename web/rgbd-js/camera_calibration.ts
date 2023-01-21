import { NativeFloatArray } from './capi_containers';

export enum CameraDeviceType {
  AZURE_KINECT = 0,
  IOS = 1,
  UNDISTORTED = 2
}

export class CameraCalibration {
  wasmModule: any;
  cameraDeviceType: CameraDeviceType;
  colorWidth: number;
  colorHeight: number;
  depthWidth: number;
  depthHeight: number;

  constructor(nativeCameraCalibration: NativeCameraCalibration) {
    this.wasmModule = nativeCameraCalibration.wasmModule;
    this.cameraDeviceType = nativeCameraCalibration.getCameraDeviceType();
    this.colorWidth = nativeCameraCalibration.getColorWidth();
    this.colorHeight = nativeCameraCalibration.getColorHeight();
    this.depthWidth = nativeCameraCalibration.getDepthWidth();
    this.depthHeight = nativeCameraCalibration.getDepthHeight();
  }

  static create(nativeCameraCalibration: NativeCameraCalibration) {
    const cameraDeviceType = nativeCameraCalibration.getCameraDeviceType();
    switch(cameraDeviceType) {
      case CameraDeviceType.AZURE_KINECT:
        return new KinectCameraCalibration(nativeCameraCalibration as NativeKinectCameraCalibration);
      case CameraDeviceType.IOS:
        return new IosCameraCalibration(nativeCameraCalibration as NativeIosCameraCalibration);
      case CameraDeviceType.UNDISTORTED:
        return new UndistortedCameraCalibration(nativeCameraCalibration as NativeUndistortedCameraCalibration);
    }
    throw Error('Failed to infer device type in CameraCalibration.create');
  }

  createNativeInstance(): NativeCameraCalibration {
    throw Error('CameraCalibration.createNativeInstance not implemented.');
  }
}

export class KinectCameraCalibration extends CameraCalibration {
  resolutionWidth: number;
  resolutionHeight: number;
  cx: number;
  cy: number;
  fx: number;
  fy: number;
  k1: number;
  k2: number;
  k3: number;
  k4: number;
  k5: number;
  k6: number;
  codx: number;
  cody: number;
  p1: number;
  p2: number;
  maxRadiusForProjection: number;

  constructor(nativeKinectCameraCalibration: NativeKinectCameraCalibration) {
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
  fx: number;
  fy: number;
  ox: number;
  oy: number;
  referenceDimensionWidth: number;
  referenceDimensionHeight: number;
  lensDistortionCenterX: number;
  lensDistortionCenterY: number;
  lensDistortionLookupTable: Float32Array;

  constructor(nativeIosCameraCalibration: NativeIosCameraCalibration) {
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

  createNativeInstance(): NativeIosCameraCalibration {
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
  fx: number;
  fy: number;
  cx: number;
  cy: number;

  constructor(nativeUndistortedCameraCalibration: NativeUndistortedCameraCalibration) {
    super(nativeUndistortedCameraCalibration);
    this.fx = nativeUndistortedCameraCalibration.getFx();
    this.fy = nativeUndistortedCameraCalibration.getFy();
    this.cx = nativeUndistortedCameraCalibration.getCx();
    this.cy = nativeUndistortedCameraCalibration.getCy();
  }

  createNativeInstance(): NativeUndistortedCameraCalibration {
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
  wasmModule: any;
  ptr: number;
  owner: boolean;

  // Do not directly use this init.
  // An instance created directly using this won't contain the class type
  // information of the C++ instance.
  // Use create() instead.
  constructor(wasmModule: any, ptr: number, owner: boolean) {
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
  static create(wasmModule: any, ptr: number, owner: boolean) {
    const cameraDeviceType = wasmModule.ccall('rgbd_camera_calibration_get_camera_device_type', 'number', ['number'], [ptr]);

    switch (cameraDeviceType) {
      case CameraDeviceType.AZURE_KINECT:
        return new NativeKinectCameraCalibration(wasmModule, ptr, owner)
      case CameraDeviceType.IOS:
        return new NativeIosCameraCalibration(wasmModule, ptr, owner)
      case CameraDeviceType.UNDISTORTED:
        return new NativeUndistortedCameraCalibration(wasmModule, ptr, owner)
    }
    throw Error('not supported camera device type found')
  }

  getCameraDeviceType(): CameraDeviceType {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_camera_device_type', 'number', ['number'], [this.ptr]);
  }

  getColorWidth(): number {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_color_width', 'number', ['number'], [this.ptr]);
  }

  getColorHeight(): number {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_color_height', 'number', ['number'], [this.ptr]);
  }

  getDepthWidth(): number {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_depth_width', 'number', ['number'], [this.ptr]);
  }

  getDepthHeight(): number {
    return this.wasmModule.ccall('rgbd_camera_calibration_get_depth_height', 'number', ['number'], [this.ptr]);
  }
}

export class NativeKinectCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  getResolutionWidth(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_resolution_width', 'number', ['number'], [this.ptr]);
  }

  getResolutionHeight(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_resolution_height', 'number', ['number'], [this.ptr]);
  }

  getCx(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_cx', 'number', ['number'], [this.ptr]);
  }

  getCy(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_cy', 'number', ['number'], [this.ptr]);
  }

  getFx(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_fx', 'number', ['number'], [this.ptr]);
  }

  getFy(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_fy', 'number', ['number'], [this.ptr]);
  }

  getK1(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k1', 'number', ['number'], [this.ptr]);
  }

  getK2(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k2', 'number', ['number'], [this.ptr]);
  }

  getK3(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k3', 'number', ['number'], [this.ptr]);
  }

  getK4(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k4', 'number', ['number'], [this.ptr]);
  }

  getK5(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k5', 'number', ['number'], [this.ptr]);
  }

  getK6(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_k6', 'number', ['number'], [this.ptr]);
  }

  getCodx(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_codx', 'number', ['number'], [this.ptr]);
  }

  getCody(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_cody', 'number', ['number'], [this.ptr]);
  }

  getP1(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_p1', 'number', ['number'], [this.ptr]);
  }

  getP2(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_p2', 'number', ['number'], [this.ptr]);
  }

  getMaxRadiusForProjection(): number {
    return this.wasmModule.ccall('rgbd_kinect_camera_calibration_get_max_radius_for_projection', 'number', ['number'], [this.ptr]);
  }
}

export class NativeIosCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  getFx(): number {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_fx', 'number', ['number'], [this.ptr]);
  }

  getFy(): number {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_fy', 'number', ['number'], [this.ptr]);
  }

  getOx(): number {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_ox', 'number', ['number'], [this.ptr]);
  }

  getOy(): number {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_oy', 'number', ['number'], [this.ptr]);
  }

  getReferenceDimensionWidth(): number {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_reference_dimension_width', 'number', ['number'], [this.ptr]);
  }

  getReferenceDimensionHeight(): number {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_reference_dimension_height', 'number', ['number'], [this.ptr]);
  }

  getLensDistortionCenterX(): number {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_lens_distortion_center_x', 'number', ['number'], [this.ptr]);
  }

  getLensDistortionCenterY(): number {
    return this.wasmModule.ccall('rgbd_ios_camera_calibration_get_lens_distortion_center_y', 'number', ['number'], [this.ptr]);
  }

  getLensDistortionLookupTable(): Float32Array {
    const nativeFloatArrayPtr = this.wasmModule.ccall('rgbd_ios_camera_calibration_get_lens_distortion_lookup_table', 'number', ['number'], [this.ptr]);
    const nativeFloatArray = new NativeFloatArray(this.wasmModule, nativeFloatArrayPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    return floatArray;
  }
}

export class NativeUndistortedCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  getFx(): number {
    return this.wasmModule.ccall('rgbd_undistorted_camera_calibration_get_fx', 'number', ['number'], [this.ptr]);
  }

  getFy(): number {
    return this.wasmModule.ccall('rgbd_undistorted_camera_calibration_get_fy', 'number', ['number'], [this.ptr]);
  }

  getCx(): number {
    return this.wasmModule.ccall('rgbd_undistorted_camera_calibration_get_cx', 'number', ['number'], [this.ptr]);
  }

  getCy(): number {
    return this.wasmModule.ccall('rgbd_undistorted_camera_calibration_get_cy', 'number', ['number'], [this.ptr]);
  }
}
