import { NativeCameraCalibration, NativeKinectCameraCalibration } from '../camera_calibration';

export declare const RGBD_CAMERA_DEVICE_TYPE_AZURE_KINECT: number;
export declare const RGBD_CAMERA_DEVICE_TYPE_IOS: number;
export declare const RGBD_CAMERA_DEVICE_TYPE_UNDISTORTED: number;

export declare class CameraCalibration {
  wasmModule: any;
  cameraDeviceType: number;
  colorWidth: number;
  colorHeight: number;
  depthWidth: number;
  depthHeight: number;

  constructor(nativeCameraCalibration: NativeCameraCalibration);
  static create(nativeCameraCalibration: NativeCameraCalibration): CameraCalibration;
  createNativeInstance(): NativeCameraCalibration;
}

export declare class IosCameraCalibration extends CameraCalibration {
  fx: number;
  fy: number;
  ox: number;
  oy: number;
  referenceDimensionWidth: number;
  referenceDimensionHeight: number;
  lensDistortionCenterX: number;
  lensDistortionCenterY: number;
  lensDistortionLookupTable: number[];

  constructor(nativeIosCameraCalibration: NativeIosCameraCalibration);
  createNativeInstance(): IosCameraCalibration;
}

export declare class KinectCameraCalibration extends CameraCalibration {
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

  constructor(nativeKinectCameraCalibration: NativeKinectCameraCalibration);
  createNativeInstance(): NativeKinectCameraCalibration;
}

export declare class UndistortedCameraCalibration extends CameraCalibration {
  fx: number;
  fy: number;
  cx: number;
  cy: number;

  constructor(nativeKinectCameraCalibration: NativeUndistortedCameraCalibration);
  createNativeInstance(): NativeKinectCameraCalibration;
}

export declare class NativeCameraCalibration {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  static create(wasmModule: any, ptr: number, owner: boolean);
  getCameraDeviceType(): number;
  getColorWidth(): number;
  getColorHeight(): number;
  getDepthWidth(): number;
  getDepthHeight(): number;
}

export declare class NativeIosCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule: any, ptr: number, owner: boolean);
  getFx(): number;
  getFy(): number;
  getOx(): number;
  getOy(): number;
  getReferenceDimensionWidth(): number;
  getReferenceDimensionHeight(): number;
  getLensDistortionCenterX(): number;
  getLensDistortionCenterY(): number;
  getLensDistortionLookupTable(): number[];
}

export declare class NativeKinectCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule: any, ptr: number, owner: boolean);
  getResolutionWidth(): number;
  getResolutionHeight(): number;
  getCx(): number;
  getCy(): number;
  getFx(): number;
  getFy(): number;
  getK1(): number;
  getK2(): number;
  getK3(): number;
  getK4(): number;
  getK5(): number;
  getK6(): number;
  getCodx(): number;
  getCody(): number;
  getP1(): number;
  getP2(): number;
  getMaxRadiusForProjection(): number;
}

export declare class NativeUndistortedCameraCalibration extends NativeCameraCalibration {
  constructor(wasmModule: any, ptr: number, owner: boolean);
  getFx(): number;
  getFy(): number;
  getCx(): number;
  getCy(): number;
}
