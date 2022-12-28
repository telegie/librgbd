import { NativeCameraCalibration } from './camera_calibration';
import { Quaternion } from './quaternion';
import { Vector3 } from './vector3';

export declare class FileInfo {
  timecodeScaleNs: number;
  durationUs: number;
  writingApp: string;

  constructor(nativeInfo: NativeFileInfo);
}

export declare class FileVideoTrack {
  trackNumber: number;
  codec: string;
  width: number;
  height: number;

  constructor(nativeFileVideoTrack: NativeFileVideoTrack);
}

export declare class FileDepthVideoTrack extends FileVideoTrack {
  depthUnit: number;

  constructor(nativeFileDepthVideoTrack: NativeFileDepthVideoTrack);
}

export declare class FileAudioTrack {
  trackNumber: number;
  samplingFrequency: number;

  constructor(nativeFileAudioTrack: NativeFileAudioTrack);
}

export declare class FileTracks {
  colorTrack: FileVideoTrack;
  depthTrack: FileDepthVideoTrack;
  audioTrack: FileAudioTrack;

  constructor(nativeFileTracks: NativeFileTracks);
}

export declare class FileAttachments {
  calibration: CameraCalibration;
  coverPngBytes: Uint8Array;

  constructor(nativeFileAttachments: NativeFileAttachments);
}

export declare class FileVideoFrame {
  globalTimecode: number;
  keyframe: boolean;
  colorBytes: Uint8Array;
  depthBytes: Uint8Array;
  floor: Plane;

  constructor(nativeFileVideoFrame: NativeFileVideoFrame);
}

export declare class FileAudioFrame {
  globalTimecode: number;
  bytes: Uint8Array;

  constructor(nativeFileAudioFrame: NativeFileAudioFrame);
}

export declare class FileIMUFrame {
  globalTimecode: number;
  acceleration: Vector3;
  rotationRate: Vector3;
  magneticField: Vector3;
  gravity: Vector3;

  constructor(nativeFileIMUFrame: NativeFileIMUFrame);
}

export declare class FileTRSFrame {
  globalTimecode: number;
  translation: Vector3;
  rotation: Quaternion;
  scale: Vector3;

  constructor(nativeFileTRSFrame: NativeFileTRSFrame);
}

export declare class File {
  info: FileInfo;
  tracks: FileTracks;
  attachments: FileAttachments;
  videoFrames: FileVideoFrame[];
  audioFrames: FileAudioFrame[];
  imuFrames: FileIMUFrame[];
  trsFrames: FileTRSFrame[];

  constructor(nativeFile: NativeFile);
}

export declare class NativeFileInfo {
  wasmModule: any;
  ptr: number;
  owner: boolean;
  
  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getTimecodeScaleNs(): number;
  getDurationUs(): number;
  getWritingApp(): string;
}

export declare class NativeFileVideoTrack {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getTrackNumber(): number;
  getCodec(): string;
  getWidth(): number;
  getHeight(): number;
}

export declare class NativeFileDepthVideoTrack extends NativeFileVideoTrack {
  constructor(wasmModule: any, ptr: number, owner: boolean);
  getDepthUnit(): number;
}

export declare class NativeFileAudioTrack {
  wasmModule: any;
  ptr: number;
  owner: boolean;
  
  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getTrackNumber(): number;
  getSamplingFrequency(): number;
}

export declare class NativeFileTracks {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getColorTrack(): NativeFileVideoTrack;
  getDepthTrack(): NativeFileDepthVideoTrack;
  getAudioTrack(): NativeFileAudioTrack;
}

export declare class NativeFileAttachments {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getCameraCalibration(): NativeCameraCalibration;
  getCoverPNGBytes(): Uint8Array;
}

export declare class NativeFileVideoFrame {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getGlobalTimecode(): number;
  getKeyframe(): boolean;
  getColorBytes(): Uint8Array;
  getDepthBytes(): Uint8Array;
  getFloor(): Plane;
}

export declare class NativeFileAudioFrame {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getGlobalTimecode(): number;
  getBytes(): Uint8Array;
}

export class NativeFileIMUFrame {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getGlobalTimecode(): number;
  getAcceleration(): Vector3;
  getRotationRate(): Vector3;
  getMagneticField(): Vector3;
  getGravity(): Vector3;
}

export class NativeFileTRSFrame {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean);
  close(): void;
  getGlobalTimecode(): number;
  getTranslation(): Vector3;
  getRotation(): Quaternion;
  getScale(): Vector3;
}

export declare class NativeFile {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number);
  close(): void;
  getInfo(): NativeFileInfo;
  getTracks(): NativeFileTracks;
  getAttachments(): NativeFileAttachments;
  getVideoFrameCount(): number;
  getVideoFrame(index): NativeFileVideoFrame;
  getAudioFrameCount(): number;
  getAudioFrame(index): NativeFileAudioFrame;
  getIMUFrameCount(): number;
  getIMUFrame(index): NativeFileIMUFrame;
  getTRSFrameCount(): number;
  getTRSFrame(index): NativeFileTRSFrame;
}
