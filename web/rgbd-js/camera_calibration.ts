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

  constructor(wasmModule: any,
              cameraDeviceType: CameraDeviceType,
              colorWidth: number,
              colorHeight: number,
              depthWidth: number,
              depthHeight: number) {
    this.wasmModule = wasmModule;
    this.cameraDeviceType = cameraDeviceType;
    this.colorWidth = colorWidth;
    this.colorHeight = colorHeight;
    this.depthWidth = depthWidth;
    this.depthHeight = depthHeight;
  }

  static fromNative(nativeCameraCalibration: NativeCameraCalibration) {
    const cameraDeviceType = nativeCameraCalibration.getCameraDeviceType();
    switch(cameraDeviceType) {
      case CameraDeviceType.AZURE_KINECT:
        return KinectCameraCalibration.fromNative(nativeCameraCalibration as NativeKinectCameraCalibration);
      case CameraDeviceType.IOS:
        return IosCameraCalibration.fromNative(nativeCameraCalibration as NativeIosCameraCalibration);
      case CameraDeviceType.UNDISTORTED:
        return new UndistortedCameraCalibration(nativeCameraCalibration as NativeUndistortedCameraCalibration);
    }
    throw Error('Failed to infer device type in fromNative.create');
  }

  toNative(): NativeCameraCalibration {
    throw Error('CameraCalibration.toNative not implemented.');
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

  constructor(wasmModule: any,
              cameraDeviceType: CameraDeviceType,
              colorWidth: number, colorHeight: number,
              depthWidth: number, depthHeight: number,
              resolutionWidth: number, resolutionHeight: number,
              cx: number, cy: number, fx: number, fy: number,
              k1: number, k2: number, k3: number, k4: number, k5: number, k6: number,
              codx: number, cody: number,
              p1: number, p2: number,
              maxRadiusForProjection: number) {
    super(wasmModule, cameraDeviceType, colorWidth, colorHeight, depthWidth, depthHeight);
    this.resolutionWidth = resolutionWidth;
    this.resolutionHeight = resolutionHeight;
    this.cx = cx;
    this.cy = cy;
    this.fx = fx;
    this.fy = fy;
    this.k1 = k1;
    this.k2 = k2;
    this.k3 = k3;
    this.k4 = k4;
    this.k5 = k5;
    this.k6 = k6;
    this.codx = codx;
    this.cody = cody;
    this.p1 = p1;
    this.p2 = p2;
    this.maxRadiusForProjection = maxRadiusForProjection;
  }

  static fromNative(nativeKinectCameraCalibration: NativeKinectCameraCalibration) {
    const cameraDeviceType = nativeKinectCameraCalibration.getCameraDeviceType();
    const colorWidth = nativeKinectCameraCalibration.getColorWidth();
    const colorHeight = nativeKinectCameraCalibration.getColorHeight();
    const depthWidth = nativeKinectCameraCalibration.getDepthWidth();
    const depthHeight = nativeKinectCameraCalibration.getDepthHeight();
    const resolutionWidth = nativeKinectCameraCalibration.getResolutionWidth();
    const resolutionHeight = nativeKinectCameraCalibration.getResolutionHeight();
    const cx = nativeKinectCameraCalibration.getCx();
    const cy = nativeKinectCameraCalibration.getCy();
    const fx = nativeKinectCameraCalibration.getFx();
    const fy = nativeKinectCameraCalibration.getFy();
    const k1 = nativeKinectCameraCalibration.getK1();
    const k2 = nativeKinectCameraCalibration.getK2();
    const k3 = nativeKinectCameraCalibration.getK3();
    const k4 = nativeKinectCameraCalibration.getK4();
    const k5 = nativeKinectCameraCalibration.getK5();
    const k6 = nativeKinectCameraCalibration.getK6();
    const codx = nativeKinectCameraCalibration.getCodx();
    const cody = nativeKinectCameraCalibration.getCody();
    const p1 = nativeKinectCameraCalibration.getP1();
    const p2 = nativeKinectCameraCalibration.getP2();
    const maxRadiusForProjection = nativeKinectCameraCalibration.getMaxRadiusForProjection();

    return new KinectCameraCalibration(nativeKinectCameraCalibration.wasmModule,
                                       cameraDeviceType,
                                       colorWidth, colorHeight,
                                       depthWidth, depthHeight,
                                       resolutionWidth, resolutionHeight,
                                       cx, cy, fx, fy,
                                       k1, k2, k3, k4, k5, k6,
                                       codx, cody,
                                       p1, p2,
                                       maxRadiusForProjection);
  }

  toNative() {
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

  constructor(wasmModule: any,
              cameraDeviceType: CameraDeviceType,
              colorWidth: number, colorHeight: number,
              depthWidth: number, depthHeight: number,
              fx: number, fy: number, ox: number, oy: number,
              referenceDimensionWidth: number, referenceDimensionHeight: number,
              lensDistortionCenterX: number, lensDistortionCenterY: number,
              lensDistortionLookupTable: Float32Array) {
    super(wasmModule, cameraDeviceType, colorWidth, colorHeight, depthWidth, depthHeight);
    this.fx = fx;
    this.fy = fy;
    this.ox = ox;
    this.oy = oy;
    this.referenceDimensionWidth = referenceDimensionWidth;
    this.referenceDimensionHeight = referenceDimensionHeight;
    this.lensDistortionCenterX = lensDistortionCenterX;
    this.lensDistortionCenterY = lensDistortionCenterY;
    this.lensDistortionLookupTable = lensDistortionLookupTable;
  }

  static fromNative(nativeIosCameraCalibration: NativeIosCameraCalibration) {
    const cameraDeviceType = nativeIosCameraCalibration.getCameraDeviceType();
    const colorWidth = nativeIosCameraCalibration.getColorWidth();
    const colorHeight = nativeIosCameraCalibration.getColorHeight();
    const depthWidth = nativeIosCameraCalibration.getDepthWidth();
    const depthHeight = nativeIosCameraCalibration.getDepthHeight();
    const fx = nativeIosCameraCalibration.getFx();
    const fy = nativeIosCameraCalibration.getFy();
    const ox = nativeIosCameraCalibration.getOx();
    const oy = nativeIosCameraCalibration.getOy();
    const referenceDimensionWidth = nativeIosCameraCalibration.getReferenceDimensionWidth();
    const referenceDimensionHeight = nativeIosCameraCalibration.getReferenceDimensionHeight();
    const lensDistortionCenterX = nativeIosCameraCalibration.getLensDistortionCenterX();
    const lensDistortionCenterY = nativeIosCameraCalibration.getLensDistortionCenterY();
    const lensDistortionLookupTable = nativeIosCameraCalibration.getLensDistortionLookupTable();
    return new IosCameraCalibration(nativeIosCameraCalibration.wasmModule,
                                    cameraDeviceType,
                                    colorWidth, colorHeight,
                                    depthWidth, depthHeight,
                                    fx, fy, ox, oy,
                                    referenceDimensionWidth, referenceDimensionHeight,
                                    lensDistortionCenterX, lensDistortionCenterY,
                                    lensDistortionLookupTable);
  }

  toNative(): NativeIosCameraCalibration {
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

  static fromNative(nativeUndistortedCameraCalibration: NativeUndistortedCameraCalibration) {
    const cameraDeviceType = nativeUndistortedCameraCalibration.getCameraDeviceType();
    const colorWidth = nativeUndistortedCameraCalibration.getColorWidth();
    const colorHeight = nativeUndistortedCameraCalibration.getColorHeight();
    const depthWidth = nativeUndistortedCameraCalibration.getDepthWidth();
    const depthHeight = nativeUndistortedCameraCalibration.getDepthHeight();
  }

  toNative(): NativeUndistortedCameraCalibration {
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
