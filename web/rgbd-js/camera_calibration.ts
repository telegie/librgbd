import RGBD from '../rgbd';
import { NativeFloatArray } from './capi_containers';
import { EmscriptenModuleEx } from './emscripten_module_ex';
import { NativeObject } from './native_object';

export enum CameraCalibrationType {
  AZURE_KINECT = 0,
  IOS = 1,
  UNDISTORTED = 2
}

export class CameraCalibration {
  cameraCalibrationType: CameraCalibrationType;
  colorWidth: number;
  colorHeight: number;
  depthWidth: number;
  depthHeight: number;

  constructor(cameraCalibrationType: CameraCalibrationType,
              colorWidth: number,
              colorHeight: number,
              depthWidth: number,
              depthHeight: number) {
    this.cameraCalibrationType = cameraCalibrationType;
    this.colorWidth = colorWidth;
    this.colorHeight = colorHeight;
    this.depthWidth = depthWidth;
    this.depthHeight = depthHeight;
  }

  static fromNative(nativeCameraCalibration: NativeCameraCalibration) {
    const cameraCalibrationType = nativeCameraCalibration.getType();
    switch(cameraCalibrationType) {
      case CameraCalibrationType.AZURE_KINECT:
        return KinectCameraCalibration.fromNative(nativeCameraCalibration as NativeKinectCameraCalibration);
      case CameraCalibrationType.IOS:
        return IosCameraCalibration.fromNative(nativeCameraCalibration as NativeIosCameraCalibration);
      case CameraCalibrationType.UNDISTORTED:
        return UndistortedCameraCalibration.fromNative(nativeCameraCalibration as NativeUndistortedCameraCalibration);
    }
    throw Error('Failed to infer device type in fromNative.create');
  }

  toNative(module: EmscriptenModuleEx): NativeCameraCalibration {
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

  constructor(colorWidth: number, colorHeight: number,
              depthWidth: number, depthHeight: number,
              resolutionWidth: number, resolutionHeight: number,
              cx: number, cy: number, fx: number, fy: number,
              k1: number, k2: number, k3: number, k4: number, k5: number, k6: number,
              codx: number, cody: number,
              p1: number, p2: number,
              maxRadiusForProjection: number) {
    super(CameraCalibrationType.AZURE_KINECT, colorWidth, colorHeight, depthWidth, depthHeight);
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
    const cameraCalibrationType = nativeKinectCameraCalibration.getType();
    if (cameraCalibrationType !== CameraCalibrationType.AZURE_KINECT)
      throw new Error("cameraCalibrationType !== CameraCalibrationType.AZURE_KINECT");
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

    return new KinectCameraCalibration(colorWidth, colorHeight,
                                       depthWidth, depthHeight,
                                       resolutionWidth, resolutionHeight,
                                       cx, cy, fx, fy,
                                       k1, k2, k3, k4, k5, k6,
                                       codx, cody,
                                       p1, p2,
                                       maxRadiusForProjection);
  }

  toNative(module: EmscriptenModuleEx): NativeKinectCameraCalibration {
    const nativePtr = module.ccall('rgbd_kinect_camera_calibration_ctor',
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
    return new NativeKinectCameraCalibration(module, nativePtr, true);
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
  inverseLensDistortionLookupTable: Float32Array;

  constructor(colorWidth: number, colorHeight: number,
              depthWidth: number, depthHeight: number,
              fx: number, fy: number, ox: number, oy: number,
              referenceDimensionWidth: number, referenceDimensionHeight: number,
              lensDistortionCenterX: number, lensDistortionCenterY: number,
              lensDistortionLookupTable: Float32Array,
              inverseLensDistortionLookupTable: Float32Array) {
    super(CameraCalibrationType.IOS, colorWidth, colorHeight, depthWidth, depthHeight);
    this.fx = fx;
    this.fy = fy;
    this.ox = ox;
    this.oy = oy;
    this.referenceDimensionWidth = referenceDimensionWidth;
    this.referenceDimensionHeight = referenceDimensionHeight;
    this.lensDistortionCenterX = lensDistortionCenterX;
    this.lensDistortionCenterY = lensDistortionCenterY;
    this.lensDistortionLookupTable = lensDistortionLookupTable;
    this.inverseLensDistortionLookupTable = inverseLensDistortionLookupTable;
  }

  static fromNative(nativeIosCameraCalibration: NativeIosCameraCalibration) {
    const cameraCalibrationType = nativeIosCameraCalibration.getType();
    if (cameraCalibrationType !== CameraCalibrationType.IOS)
      throw new Error("cameraCalibrationType !== CameraCalibrationType.IOS");
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
    const inverseLensDistortionLookupTable = nativeIosCameraCalibration.getInverseLensDistortionLookupTable();
    return new IosCameraCalibration(colorWidth, colorHeight,
                                    depthWidth, depthHeight,
                                    fx, fy, ox, oy,
                                    referenceDimensionWidth, referenceDimensionHeight,
                                    lensDistortionCenterX, lensDistortionCenterY,
                                    lensDistortionLookupTable,
                                    inverseLensDistortionLookupTable);
  }

  toNative(module: EmscriptenModuleEx): NativeIosCameraCalibration {
    const lookupTablePtr = module._malloc(this.lensDistortionLookupTable.byteLength);
    const inverseLookupTablePtr = module._malloc(this.inverseLensDistortionLookupTable.byteLength);
    // Have to do >> 2 to the pointer since the set() function interprets its second parameter
    // as an index, not a pointer.
    // https://github.com/emscripten-core/emscripten/issues/4003
    module.HEAPF32.set(this.lensDistortionLookupTable, lookupTablePtr >> 2);
    module.HEAPF32.set(this.inverseLensDistortionLookupTable, inverseLookupTablePtr >> 2);
    const nativePtr = module.ccall('rgbd_ios_camera_calibration_ctor',
                                   'number',
                                   ['number', 'number', 'number', 'number',
                                    'number', 'number', 'number', 'number',
                                    'number', 'number',
                                    'number', 'number',
                                    'number', 'number',
                                    'number', 'number'],
                                   [this.colorWidth, this.colorHeight, this.depthWidth, this.depthHeight,
                                    this.fx, this.fy, this.ox, this.oy,
                                    this.referenceDimensionWidth, this.referenceDimensionHeight,
                                    this.lensDistortionCenterX, this.lensDistortionCenterY,
                                    lookupTablePtr, this.lensDistortionLookupTable.length,
                                    inverseLookupTablePtr, this.inverseLensDistortionLookupTable.length]);
    module._free(lookupTablePtr);
    module._free(inverseLookupTablePtr);

    return new NativeIosCameraCalibration(module, nativePtr, true);
  }
}

export class UndistortedCameraCalibration extends CameraCalibration {
  fx: number;
  fy: number;
  cx: number;
  cy: number;

  constructor(colorWidth: number, colorHeight: number,
              depthWidth: number, depthHeight: number,
              fx: number, fy: number, cx: number, cy: number) {
    super(RGBD.CameraCalibrationType.UNDISTORTED, colorWidth, colorHeight, depthWidth, depthHeight);
    this.fx = fx;
    this.fy = fy;
    this.cx = cx;
    this.cy = cy;
  }

  static fromNative(nativeUndistortedCameraCalibration: NativeUndistortedCameraCalibration) {
    const cameraCalibrationType = nativeUndistortedCameraCalibration.getType();
    if (cameraCalibrationType !== CameraCalibrationType.UNDISTORTED)
      throw new Error("cameraCalibrationType !== CameraCalibrationType.UNDISTORTED");
    const colorWidth = nativeUndistortedCameraCalibration.getColorWidth();
    const colorHeight = nativeUndistortedCameraCalibration.getColorHeight();
    const depthWidth = nativeUndistortedCameraCalibration.getDepthWidth();
    const depthHeight = nativeUndistortedCameraCalibration.getDepthHeight();
    const fx = nativeUndistortedCameraCalibration.getFx();
    const fy = nativeUndistortedCameraCalibration.getFy();
    const cx = nativeUndistortedCameraCalibration.getCx();
    const cy = nativeUndistortedCameraCalibration.getCy();
    return new UndistortedCameraCalibration(colorWidth, colorHeight,
                                            depthWidth, depthHeight,
                                            fx, fy, cx, cy);
  }

  toNative(module: EmscriptenModuleEx): NativeUndistortedCameraCalibration {
    const nativePtr = module.ccall('rgbd_undistorted_camera_calibration_ctor',
                                   'number',
                                   ['number', 'number', 'number', 'number',
                                    'number', 'number', 'number', 'number'],
                                   [this.colorWidth, this.colorHeight, this.depthWidth, this.depthHeight,
                                    this.fx, this.fy, this.cx, this.cy]);
    return new NativeUndistortedCameraCalibration(module, nativePtr, true);
  }
}

export class NativeCameraCalibration extends NativeObject {
  // Do not directly use this init.
  // An instance created directly using this won't contain the class type
  // information of the C++ instance.
  // Use create() instead.
  constructor(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    super(module, ptr, owner);
  }

  delete() {
    this.getModule().ccall('rgbd_camera_calibration_dtor', null, ['number'], [this.getPtr()]);
  }

  // Use this instead of the raw init to create an instance
  // containing the class information matching its corresponding C++ instance.
  static create(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    const cameraCalibrationType = module.ccall('rgbd_camera_calibration_get_type', 'number', ['number'], [ptr]);

    switch (cameraCalibrationType) {
      case CameraCalibrationType.AZURE_KINECT:
        return new NativeKinectCameraCalibration(module, ptr, owner)
      case CameraCalibrationType.IOS:
        return new NativeIosCameraCalibration(module, ptr, owner)
      case CameraCalibrationType.UNDISTORTED:
        return new NativeUndistortedCameraCalibration(module, ptr, owner)
    }
    throw Error('not supported camera device type found')
  }

  getType(): CameraCalibrationType {
    return this.getModule().ccall('rgbd_camera_calibration_get_type', 'number', ['number'], [this.getPtr()]);
  }

  getColorWidth(): number {
    return this.getModule().ccall('rgbd_camera_calibration_get_color_width', 'number', ['number'], [this.getPtr()]);
  }

  getColorHeight(): number {
    return this.getModule().ccall('rgbd_camera_calibration_get_color_height', 'number', ['number'], [this.getPtr()]);
  }

  getDepthWidth(): number {
    return this.getModule().ccall('rgbd_camera_calibration_get_depth_width', 'number', ['number'], [this.getPtr()]);
  }

  getDepthHeight(): number {
    return this.getModule().ccall('rgbd_camera_calibration_get_depth_height', 'number', ['number'], [this.getPtr()]);
  }
}

export class NativeKinectCameraCalibration extends NativeCameraCalibration {
  constructor(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    super(module, ptr, owner);
  }

  getResolutionWidth(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_resolution_width', 'number', ['number'], [this.getPtr()]);
  }

  getResolutionHeight(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_resolution_height', 'number', ['number'], [this.getPtr()]);
  }

  getCx(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_cx', 'number', ['number'], [this.getPtr()]);
  }

  getCy(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_cy', 'number', ['number'], [this.getPtr()]);
  }

  getFx(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_fx', 'number', ['number'], [this.getPtr()]);
  }

  getFy(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_fy', 'number', ['number'], [this.getPtr()]);
  }

  getK1(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_k1', 'number', ['number'], [this.getPtr()]);
  }

  getK2(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_k2', 'number', ['number'], [this.getPtr()]);
  }

  getK3(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_k3', 'number', ['number'], [this.getPtr()]);
  }

  getK4(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_k4', 'number', ['number'], [this.getPtr()]);
  }

  getK5(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_k5', 'number', ['number'], [this.getPtr()]);
  }

  getK6(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_k6', 'number', ['number'], [this.getPtr()]);
  }

  getCodx(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_codx', 'number', ['number'], [this.getPtr()]);
  }

  getCody(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_cody', 'number', ['number'], [this.getPtr()]);
  }

  getP1(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_p1', 'number', ['number'], [this.getPtr()]);
  }

  getP2(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_p2', 'number', ['number'], [this.getPtr()]);
  }

  getMaxRadiusForProjection(): number {
    return this.getModule().ccall('rgbd_kinect_camera_calibration_get_max_radius_for_projection', 'number', ['number'], [this.getPtr()]);
  }
}

export class NativeIosCameraCalibration extends NativeCameraCalibration {
  constructor(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    super(module, ptr, owner);
  }

  getFx(): number {
    return this.getModule().ccall('rgbd_ios_camera_calibration_get_fx', 'number', ['number'], [this.getPtr()]);
  }

  getFy(): number {
    return this.getModule().ccall('rgbd_ios_camera_calibration_get_fy', 'number', ['number'], [this.getPtr()]);
  }

  getOx(): number {
    return this.getModule().ccall('rgbd_ios_camera_calibration_get_ox', 'number', ['number'], [this.getPtr()]);
  }

  getOy(): number {
    return this.getModule().ccall('rgbd_ios_camera_calibration_get_oy', 'number', ['number'], [this.getPtr()]);
  }

  getReferenceDimensionWidth(): number {
    return this.getModule().ccall('rgbd_ios_camera_calibration_get_reference_dimension_width', 'number', ['number'], [this.getPtr()]);
  }

  getReferenceDimensionHeight(): number {
    return this.getModule().ccall('rgbd_ios_camera_calibration_get_reference_dimension_height', 'number', ['number'], [this.getPtr()]);
  }

  getLensDistortionCenterX(): number {
    return this.getModule().ccall('rgbd_ios_camera_calibration_get_lens_distortion_center_x', 'number', ['number'], [this.getPtr()]);
  }

  getLensDistortionCenterY(): number {
    return this.getModule().ccall('rgbd_ios_camera_calibration_get_lens_distortion_center_y', 'number', ['number'], [this.getPtr()]);
  }

  getLensDistortionLookupTable(): Float32Array {
    const nativeFloatArrayPtr = this.getModule().ccall('rgbd_ios_camera_calibration_get_lens_distortion_lookup_table', 'number', ['number'], [this.getPtr()]);
    const nativeFloatArray = new NativeFloatArray(this.getModule(), nativeFloatArrayPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    return floatArray;
  }

  getInverseLensDistortionLookupTable(): Float32Array {
    const nativeFloatArrayPtr = this.getModule().ccall('rgbd_ios_camera_calibration_get_inverse_lens_distortion_lookup_table', 'number', ['number'], [this.getPtr()]);
    const nativeFloatArray = new NativeFloatArray(this.getModule(), nativeFloatArrayPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    return floatArray;
  }
}

export class NativeUndistortedCameraCalibration extends NativeCameraCalibration {
  constructor(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    super(module, ptr, owner);
  }

  getFx(): number {
    return this.getModule().ccall('rgbd_undistorted_camera_calibration_get_fx', 'number', ['number'], [this.getPtr()]);
  }

  getFy(): number {
    return this.getModule().ccall('rgbd_undistorted_camera_calibration_get_fy', 'number', ['number'], [this.getPtr()]);
  }

  getCx(): number {
    return this.getModule().ccall('rgbd_undistorted_camera_calibration_get_cx', 'number', ['number'], [this.getPtr()]);
  }

  getCy(): number {
    return this.getModule().ccall('rgbd_undistorted_camera_calibration_get_cy', 'number', ['number'], [this.getPtr()]);
  }
}
