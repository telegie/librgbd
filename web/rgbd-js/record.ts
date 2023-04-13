import { CameraCalibration, NativeCameraCalibration } from './camera_calibration';
import { NativeByteArray, NativeString } from './capi_containers';
import { DepthCodecType } from './depth_decoder';
import { ColorCodecType } from './color_decoder';
import { DirectionTable, NativeDirectionTable } from './direction_table';
import { Quaternion, Vector3 } from '@math.gl/core';
import { NativeObject } from './native_object';

export class RecordInfo {
  timecodeScaleNs: number;
  durationUs: number;
  writingApp: string;

  constructor(timecodeScaleNs: number,
              durationUs: number,
              writingApp: string) {
    this.timecodeScaleNs = timecodeScaleNs;
    this.durationUs = durationUs;
    this.writingApp = writingApp;
  }

  static fromNative(nativeInfo: NativeRecordInfo) {
    const timecodeScaleNs = nativeInfo.getTimecodeScaleNs();
    const durationUs = nativeInfo.getDurationUs();
    const writingApp = nativeInfo.getWritingApp();
    return new RecordInfo(timecodeScaleNs, durationUs, writingApp);
  }
}

export class RecordVideoTrack {
  trackNumber: number;
  width: number;
  height: number;

  constructor(trackNumber: number,
              width: number,
              height: number) {
    this.trackNumber = trackNumber;
    this.width = width;
    this.height = height;
  }

  static fromNative(nativeRecordVideoTrack: NativeRecordVideoTrack) {
    const trackNumber = nativeRecordVideoTrack.getTrackNumber();
    const width = nativeRecordVideoTrack.getWidth();
    const height = nativeRecordVideoTrack.getHeight();
    return new RecordVideoTrack(trackNumber, width, height);
  }
}

export class RecordColorVideoTrack extends RecordVideoTrack {
  codec: ColorCodecType;

  constructor(trackNumber: number,
              width: number,
              height: number,
              codec: ColorCodecType) {
    super(trackNumber, width, height);
    this.codec = codec;
  }

  static fromNative(nativeRecordColorVideoTrack: NativeRecordColorVideoTrack) {
    const trackNumber = nativeRecordColorVideoTrack.getTrackNumber();
    const width = nativeRecordColorVideoTrack.getWidth();
    const height = nativeRecordColorVideoTrack.getHeight();
    const codec = nativeRecordColorVideoTrack.getCodec();
    return new RecordColorVideoTrack(trackNumber, width, height, codec);
  }
}

export class RecordDepthVideoTrack extends RecordVideoTrack {
  codec: DepthCodecType;
  depthUnit: number;

  constructor(trackNumber: number,
              width: number,
              height: number,
              codec: DepthCodecType,
              depthUnit: number ) {
    super(trackNumber, width, height);
    this.codec = codec;
    this.depthUnit = depthUnit;
  }

  static fromNative(nativeRecordDepthVideoTrack: NativeRecordDepthVideoTrack) {
    const trackNumber = nativeRecordDepthVideoTrack.getTrackNumber();
    const width = nativeRecordDepthVideoTrack.getWidth();
    const height = nativeRecordDepthVideoTrack.getHeight();
    const codec = nativeRecordDepthVideoTrack.getCodec();
    const depthUnit = nativeRecordDepthVideoTrack.getDepthUnit();
    return new RecordDepthVideoTrack(trackNumber, width, height, codec, depthUnit);
  }
}

export class RecordAudioTrack {
  trackNumber: number;
  samplingFrequency: number;

  constructor(trackNumber: number,
              samplingFrequency: number) {
    this.trackNumber = trackNumber;
    this.samplingFrequency = samplingFrequency;
  }

  static fromNative(nativeRecordAudioTrack: NativeRecordAudioTrack) {
    const trackNumber = nativeRecordAudioTrack.getTrackNumber();
    const samplingFrequency = nativeRecordAudioTrack.getSamplingFrequency();
    return new RecordAudioTrack(trackNumber, samplingFrequency);
  }
}

export class RecordTracks {
  colorTrack: RecordColorVideoTrack;
  depthTrack: RecordDepthVideoTrack;
  audioTrack: RecordAudioTrack;

  constructor(colorTrack: RecordColorVideoTrack,
              depthTrack: RecordDepthVideoTrack,
              audioTrack: RecordAudioTrack) {
    this.colorTrack = colorTrack;
    this.depthTrack = depthTrack;
    this.audioTrack = audioTrack;
  }

  static fromNative(nativeRecordTracks: NativeRecordTracks) {
    const nativeColorTrack = nativeRecordTracks.getColorTrack()
    const colorTrack = RecordColorVideoTrack.fromNative(nativeColorTrack);
    nativeColorTrack.close();

    const nativeDepthTrack = nativeRecordTracks.getDepthTrack();
    const depthTrack = RecordDepthVideoTrack.fromNative(nativeDepthTrack);
    nativeDepthTrack.close();

    const nativeAudioTrack = nativeRecordTracks.getAudioTrack();
    const audioTrack = RecordAudioTrack.fromNative(nativeAudioTrack);
    nativeAudioTrack.close();

    return new RecordTracks(colorTrack, depthTrack, audioTrack);
  }
}

export class RecordAttachments {
  calibration: CameraCalibration;
  coverPngBytes: Uint8Array | null;

  constructor(calibration: CameraCalibration,
              coverPngBytes: Uint8Array | null) {
    this.calibration = calibration;
    this.coverPngBytes = coverPngBytes;
  }

  static fromNative(nativeRecordAttachments: NativeRecordAttachments) {
    const nativeCalibration = nativeRecordAttachments.getCameraCalibration();
    const calibration = CameraCalibration.fromNative(nativeCalibration);
    nativeCalibration.close();

    const coverPngBytes = nativeRecordAttachments.getCoverPNGBytes();
    return new RecordAttachments(calibration, coverPngBytes);
  }
}

export class RecordVideoFrame {
  timePointUs: number;
  keyframe: boolean;
  colorBytes: Uint8Array;
  depthBytes: Uint8Array;

  constructor(timePointUs: number,
              keyframe: boolean,
              colorBytes: Uint8Array,
              depthBytes: Uint8Array) {
    this.timePointUs = timePointUs;
    this.keyframe = keyframe;
    this.colorBytes = colorBytes;
    this.depthBytes = depthBytes;
  }

  static fromNative(nativeRecordVideoFrame: NativeRecordVideoFrame) {
    const timePointUs = nativeRecordVideoFrame.getTimePointUs();
    const keyframe = nativeRecordVideoFrame.getKeyframe();
    const colorBytes = nativeRecordVideoFrame.getColorBytes();
    const depthBytes = nativeRecordVideoFrame.getDepthBytes();
    return new RecordVideoFrame(timePointUs, keyframe, colorBytes, depthBytes);
  }

  toNative(wasmModule: any) {
    const colorBytesPtr = wasmModule._malloc(this.colorBytes.byteLength);
    const depthBytesPtr = wasmModule._malloc(this.depthBytes.byteLength);
    wasmModule.HEAPU8.set(this.colorBytes, colorBytesPtr);
    wasmModule.HEAPU8.set(this.depthBytes, depthBytesPtr);
    const ptr = wasmModule.ccall('rgbd_record_video_frame_ctor_wasm',
                                 'number',
                                 ['number', 'number', 'number', 'number', 'number', 'number'],
                                 [this.timePointUs, this.keyframe,
                                  colorBytesPtr, this.colorBytes.byteLength,
                                  depthBytesPtr, this.depthBytes.byteLength]);
    wasmModule._free(colorBytesPtr);
    wasmModule._free(depthBytesPtr);

    return new NativeRecordVideoFrame(wasmModule, ptr, true);
  }
}

export class RecordAudioFrame {
  timePointUs: number;
  bytes: Uint8Array;

  constructor(timePointUs: number, bytes: Uint8Array) {
    this.timePointUs = timePointUs;
    this.bytes = bytes;
  }

  static fromNative(nativeRecordAudioFrame: NativeRecordAudioFrame) {
    const timePointUs = nativeRecordAudioFrame.getTimePointUs();
    const bytes = nativeRecordAudioFrame.getBytes();
    return new RecordAudioFrame(timePointUs, bytes);
  }

  toNative(wasmModule: any) {
    const bytesPtr = wasmModule._malloc(this.bytes.byteLength);
    wasmModule.HEAPU8.set(this.bytes, bytesPtr);
    const ptr = wasmModule.ccall('rgbd_record_audio_frame_ctor_wasm',
                                 'number',
                                 ['number', 'number', 'number'],
                                 [this.timePointUs, bytesPtr, this.bytes.byteLength]);
    wasmModule._free(bytesPtr);

    return new NativeRecordAudioFrame(wasmModule, ptr, true);
  }
}

export class RecordIMUFrame {
  timePointUs: number;
  acceleration: Vector3;
  rotationRate: Vector3;
  magneticField: Vector3;
  gravity: Vector3;

  constructor(timePointUs: number,
              acceleration: Vector3,
              rotationRate: Vector3,
              magneticField: Vector3,
              gravity: Vector3) {
    this.timePointUs = timePointUs;
    this.acceleration = acceleration;
    this.rotationRate = rotationRate;
    this.magneticField = magneticField;
    this.gravity = gravity;
  }

  static fromNative(nativeRecordIMUFrame: NativeRecordIMUFrame) {
    const timePointUs = nativeRecordIMUFrame.getTimePointUs();
    const acceleration = nativeRecordIMUFrame.getAcceleration();
    const rotationRate = nativeRecordIMUFrame.getRotationRate();
    const magneticField = nativeRecordIMUFrame.getMagneticField();
    const gravity = nativeRecordIMUFrame.getGravity();
    return new RecordIMUFrame(timePointUs,
                            acceleration,
                            rotationRate,
                            magneticField,
                            gravity)
  }

  toNative(wasmModule: any): NativeRecordIMUFrame {
    const ptr = wasmModule.ccall('rgbd_record_imu_frame_ctor_wasm',
                                 'number',
                                 ['number',
                                  'number', 'number', 'number',
                                  'number', 'number', 'number',
                                  'number', 'number', 'number',
                                  'number', 'number', 'number'],
                                 [this.timePointUs,
                                  this.acceleration.x, this.acceleration.y, this.acceleration.z,
                                  this.rotationRate.x, this.rotationRate.y, this.rotationRate.z,
                                  this.magneticField.x, this.magneticField.y, this.magneticField.z,
                                  this.gravity.x, this.gravity.y, this.gravity.z]);

    return new NativeRecordIMUFrame(wasmModule, ptr, true);
  }
}

export class RecordPoseFrame {
  timePointUs: number;
  translation: Vector3;
  rotation: Quaternion;

  constructor(timePointUs: number,
              translation: Vector3,
              rotation: Quaternion) {
    this.timePointUs = timePointUs;
    this.translation = translation;
    this.rotation = rotation;
  }

  static fromNative(nativeRecordPoseFrame: NativeRecordPoseFrame) {
    const timePointUs = nativeRecordPoseFrame.getTimePointUs();
    const translation = nativeRecordPoseFrame.getTranslation();
    const rotation = nativeRecordPoseFrame.getRotation();
    return new RecordPoseFrame(timePointUs, translation, rotation);
  }

  toNative(wasmModule: any): NativeRecordPoseFrame {
    const ptr = wasmModule.ccall('rgbd_record_pose_frame_ctor_wasm',
                                 'number',
                                 ['number',
                                  'number', 'number', 'number',
                                  'number', 'number', 'number', 'number'],
                                 [this.timePointUs,
                                  this.translation.x, this.translation.y, this.translation.z,
                                  this.rotation.w, this.rotation.x, this.rotation.y, this.rotation.z]);

    return new NativeRecordPoseFrame(wasmModule, ptr, true);
  }
}

export class RecordCalibrationFrame {
  timePointUs: number;
  cameraCalibration: CameraCalibration;

  constructor(timePointUs: number,
              cameraCalibration: CameraCalibration) {
    this.timePointUs = timePointUs;
    this.cameraCalibration = cameraCalibration;
  }

  static fromNative(nativeRecordCalibrationFrame: NativeRecordCalibrationFrame) {
    const timePointUs = nativeRecordCalibrationFrame.getTimePointUs();
    const nativeCalibration = nativeRecordCalibrationFrame.getCameraCalibration();
    const calibration = CameraCalibration.fromNative(nativeCalibration);
    nativeCalibration.close();

    return new RecordCalibrationFrame(timePointUs, calibration);
  }

  toNative(wasmModule: any): NativeRecordCalibrationFrame {
    const ptr = wasmModule.ccall('rgbd_record_calibration_frame_ctor_wasm',
                                 'number',
                                 ['number', 'number'],
                                 [this.timePointUs, this.cameraCalibration]);

    return new NativeRecordCalibrationFrame(wasmModule, ptr, true);
  }
}

export class Record {
  info: RecordInfo;
  tracks: RecordTracks;
  attachments: RecordAttachments;
  videoFrames: RecordVideoFrame[];
  audioFrames: RecordAudioFrame[];
  imuFrames: RecordIMUFrame[];
  poseFrames: RecordPoseFrame[];
  calibrationFrames: RecordCalibrationFrame[];
  directionTable: DirectionTable | null;

  constructor(info: RecordInfo,
              tracks: RecordTracks,
              attachments: RecordAttachments,
              videoFrames: RecordVideoFrame[],
              audioFrames: RecordAudioFrame[],
              imuFrames: RecordIMUFrame[],
              poseFrames: RecordPoseFrame[],
              calibrationFrames: RecordCalibrationFrame[],
              directionTable: DirectionTable | null) {
    this.info = info;
    this.tracks = tracks;
    this.attachments = attachments;
    this.videoFrames = videoFrames;
    this.audioFrames = audioFrames;
    this.imuFrames = imuFrames;
    this.poseFrames = poseFrames;
    this.calibrationFrames = calibrationFrames;
    this.directionTable = directionTable;
  }

  static fromNative(nativeRecord: NativeRecord) {
    const nativeInfo = nativeRecord.getInfo();
    const info = RecordInfo.fromNative(nativeInfo);
    nativeInfo.close();

    const nativeTracks = nativeRecord.getTracks();
    const tracks = RecordTracks.fromNative(nativeTracks);
    nativeTracks.close();

    const nativeAttachments = nativeRecord.getAttachments();
    const attachments = RecordAttachments.fromNative(nativeAttachments);
    nativeAttachments.close();

    let videoFrames: RecordVideoFrame[] = [];
    const videoFrameCount = nativeRecord.getVideoFrameCount();
    for (let i = 0; i < videoFrameCount; i++) {
      const nativeVideoFrame = nativeRecord.getVideoFrame(i);
      videoFrames.push(RecordVideoFrame.fromNative(nativeVideoFrame));
      nativeVideoFrame.close();
    }

    let audioFrames: RecordAudioFrame[] = [];
    const audioFrameCount = nativeRecord.getAudioFrameCount();
    for (let i = 0; i < audioFrameCount; i++) {
      const nativeAudioFrame = nativeRecord.getAudioFrame(i);
      audioFrames.push(RecordAudioFrame.fromNative(nativeAudioFrame));
      nativeAudioFrame.close();
    }

    let imuFrames: RecordIMUFrame[] = [];
    const imuFrameCount = nativeRecord.getIMUFrameCount();
    for (let i = 0; i < imuFrameCount; i++) {
      const nativeIMUFrame = nativeRecord.getIMUFrame(i);
      imuFrames.push(RecordIMUFrame.fromNative(nativeIMUFrame));
      nativeIMUFrame.close();
    }

    let poseFrames: RecordPoseFrame[] = [];
    const poseFrameCount = nativeRecord.getPoseFrameCount();
    for (let i = 0; i < poseFrameCount; i++) {
      const nativePoseFrame = nativeRecord.getPoseFrame(i);
      poseFrames.push(RecordPoseFrame.fromNative(nativePoseFrame));
      nativePoseFrame.close();
    }

    let calibrationFrames: RecordCalibrationFrame[] = [];
    const calibrationFrameCount = nativeRecord.getCalibrationFrameCount();
    for (let i = 0; i < calibrationFrameCount; i++) {
      const nativeCalibrationFrame = nativeRecord.getCalibrationFrame(i);
      calibrationFrames.push(RecordCalibrationFrame.fromNative(nativeCalibrationFrame));
      nativeCalibrationFrame.close();
    }

    let directionTable: DirectionTable | null = null;
    if (nativeRecord.hasDirectionTable()) {
      const nativeDirectionTable = nativeRecord.getDirectionTable();
      directionTable = DirectionTable.fromNative(nativeDirectionTable);
    }
    
    return new Record(info, tracks, attachments,
      videoFrames, audioFrames, imuFrames, poseFrames, calibrationFrames, directionTable);
  }

  // This function does a shallow copy.
  clone(): Record {
    return new Record(this.info, this.tracks, this.attachments,
      this.videoFrames, this.audioFrames, this.imuFrames, this.poseFrames, this.calibrationFrames, this.directionTable);
  }
}

export class NativeRecordInfo extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_info_dtor', null, ['number'], [this.ptr]);
  }

  getTimecodeScaleNs(): number {
    return this.wasmModule.ccall('rgbd_record_info_get_timecode_scale_ns', 'number', ['number'], [this.ptr]);
  }

  getDurationUs(): number {
    return this.wasmModule.ccall('rgbd_record_info_get_duration_us', 'number', ['number'], [this.ptr]);
  }

  getWritingApp(): string {
    const nativeStrPtr = this.wasmModule.ccall('rgbd_record_info_get_writing_app', 'string', ['number'], [this.ptr]);
    const nativeStr = new NativeString(this.wasmModule, nativeStrPtr);
    const writingApp = nativeStr.toString();
    nativeStr.close();
    return writingApp;
  }
}

export class NativeRecordVideoTrack extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_video_track_dtor', 'null', ['number'], [this.ptr]);
  }

  getTrackNumber(): number {
    return this.wasmModule.ccall('rgbd_record_video_track_get_track_number', 'number', ['number'], [this.ptr]);
  }

  getWidth(): number {
    return this.wasmModule.ccall('rgbd_record_video_track_get_width', 'number', ['number'], [this.ptr]);
  }

  getHeight(): number {
    return this.wasmModule.ccall('rgbd_record_video_track_get_height', 'number', ['number'], [this.ptr]);
  }
}

export class NativeRecordColorVideoTrack extends NativeRecordVideoTrack {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  getCodec(): ColorCodecType {
    return this.wasmModule.ccall('rgbd_record_color_video_track_get_codec', 'number', ['number'], [this.ptr]);
  }
}

export class NativeRecordDepthVideoTrack extends NativeRecordVideoTrack {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  getCodec(): DepthCodecType {
    return this.wasmModule.ccall('rgbd_record_depth_video_track_get_codec', 'number', ['number'], [this.ptr]);
  }

  getDepthUnit(): number {
    return this.wasmModule.ccall('rgbd_record_depth_video_track_get_depth_unit', 'number', ['number'], [this.ptr]);
  }
}

export class NativeRecordAudioTrack extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_audio_track_dtor', 'null', ['number'], [this.ptr]);
  }

  getTrackNumber(): number {
    return this.wasmModule.ccall('rgbd_record_audio_track_get_track_number', 'number', ['number'], [this.ptr]);
  }

  getSamplingFrequency(): number {
    return this.wasmModule.ccall('rgbd_record_audio_track_get_sampling_frequency', 'number', ['number'], [this.ptr]);
  }
}

export class NativeRecordTracks extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_tracks_dtor', null, ['number'], [this.ptr]);
  }

  getColorTrack(): NativeRecordColorVideoTrack {
    const trackPtr = this.wasmModule.ccall('rgbd_record_tracks_get_color_track', 'number', ['number'], [this.ptr]);
    return new NativeRecordColorVideoTrack(this.wasmModule, trackPtr, false);
  }

  getDepthTrack(): NativeRecordDepthVideoTrack {
    const trackPtr = this.wasmModule.ccall('rgbd_record_tracks_get_depth_track', 'number', ['number'], [this.ptr]);
    return new NativeRecordDepthVideoTrack(this.wasmModule, trackPtr, false);
  }

  getAudioTrack(): NativeRecordAudioTrack {
    const trackPtr = this.wasmModule.ccall('rgbd_record_tracks_get_audio_track', 'number', ['number'], [this.ptr]);
    return new NativeRecordAudioTrack(this.wasmModule, trackPtr, false);
  }
}

export class NativeRecordAttachments extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_attachments_dtor', null, ['number'], [this.ptr]);
  }

  getCameraCalibration(): NativeCameraCalibration {
    const calibrationPtr = this.wasmModule.ccall('rgbd_record_attachments_get_camera_calibration', 'number', ['number'], [this.ptr]);
    return NativeCameraCalibration.create(this.wasmModule, calibrationPtr, false);
  }

  getCoverPNGBytes(): Uint8Array | null {
    const bytesPtr = this.wasmModule.ccall('rgbd_record_attachments_get_cover_png_bytes', 'number', ['number'], [this.ptr]);
    if (bytesPtr == 0)
      return null;

    const nativeByteArray = new NativeByteArray(this.wasmModule, bytesPtr);
    const bytes = nativeByteArray.toArray();
    nativeByteArray.close();
    return bytes;
  }
}

export class NativeRecordVideoFrame extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_video_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_record_video_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getKeyframe(): boolean {
    return this.wasmModule.ccall('rgbd_record_video_frame_get_keyframe', 'boolean', ['number'], [this.ptr]);
  }

  getColorBytes(): Uint8Array {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_record_video_frame_get_color_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }

  getDepthBytes(): Uint8Array {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_record_video_frame_get_depth_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }
}

export class NativeRecordAudioFrame extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_audio_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_record_audio_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getBytes(): Uint8Array {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_record_audio_frame_get_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }
}

export class NativeRecordIMUFrame extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_imu_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_record_imu_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getAcceleration(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_record_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_record_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_record_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getRotationRate(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_record_imu_frame_get_rotation_rate_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_record_imu_frame_get_rotation_rate_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_record_imu_frame_get_rotation_rate_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getMagneticField(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_record_imu_frame_get_magnetic_field_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_record_imu_frame_get_magnetic_field_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_record_imu_frame_get_magnetic_field_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getGravity(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_record_imu_frame_get_gravity_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_record_imu_frame_get_gravity_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_record_imu_frame_get_gravity_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }
}

export class NativeRecordPoseFrame extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_pose_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_record_pose_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getTranslation(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_record_pose_frame_get_translation_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_record_pose_frame_get_translation_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_record_pose_frame_get_translation_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getRotation(): Quaternion {
    const w  = this.wasmModule.ccall('rgbd_record_pose_frame_get_rotation_w', 'number', ['number'], [this.ptr]);
    const x  = this.wasmModule.ccall('rgbd_record_pose_frame_get_rotation_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_record_pose_frame_get_rotation_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_record_pose_frame_get_rotation_z', 'number', ['number'], [this.ptr]);
    return new Quaternion(x, y, z, w);
  }
}

export class NativeRecordCalibrationFrame extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_calibration_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_record_calibration_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getCameraCalibration(): NativeCameraCalibration {
    const calibrationPtr = this.wasmModule.ccall('rgbd_record_calibration_frame_get_camera_calibration', 'number', ['number'], [this.ptr]);
    return NativeCameraCalibration.create(this.wasmModule, calibrationPtr, false);
  }
}

export class NativeRecord extends NativeObject {
  constructor(wasmModule: any, ptr: number) {
    super(wasmModule, ptr, true);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_dtor', null, ['number'], [this.ptr]);
  }

  getInfo(): NativeRecordInfo {
    const infoPtr = this.wasmModule.ccall('rgbd_record_get_info', 'number', ['number'], [this.ptr]);
    return new NativeRecordInfo(this.wasmModule, infoPtr, false);
  }

  getTracks(): NativeRecordTracks {
    const tracksPtr = this.wasmModule.ccall('rgbd_record_get_tracks', 'number', ['number'], [this.ptr]);
    return new NativeRecordTracks(this.wasmModule, tracksPtr, false);
  }

  getAttachments(): NativeRecordAttachments {
    const attachmentsPtr = this.wasmModule.ccall('rgbd_record_get_attachments', 'number', ['number'], [this.ptr]);
    return new NativeRecordAttachments(this.wasmModule, attachmentsPtr, false);
  }

  getVideoFrameCount(): number {
    return this.wasmModule.ccall('rgbd_record_get_video_frame_count', 'number', ['number'], [this.ptr]);
  }

  getVideoFrame(index: number): NativeRecordVideoFrame {
    const videoFramePtr = this.wasmModule.ccall('rgbd_record_get_video_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeRecordVideoFrame(this.wasmModule, videoFramePtr, false);
  }

  getAudioFrameCount(): number {
    return this.wasmModule.ccall('rgbd_record_get_audio_frame_count', 'number', ['number'], [this.ptr]);
  }

  getAudioFrame(index: number): NativeRecordAudioFrame {
    const audioFramePtr = this.wasmModule.ccall('rgbd_record_get_audio_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeRecordAudioFrame(this.wasmModule, audioFramePtr, false);
  }

  getIMUFrameCount(): number {
    return this.wasmModule.ccall('rgbd_record_get_imu_frame_count', 'number', ['number'], [this.ptr]);
  }

  getIMUFrame(index: number): NativeRecordIMUFrame {
    const imuFramePtr = this.wasmModule.ccall('rgbd_record_get_imu_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeRecordIMUFrame(this.wasmModule, imuFramePtr, false);
  }

  getPoseFrameCount(): number {
    return this.wasmModule.ccall('rgbd_record_get_pose_frame_count', 'number', ['number'], [this.ptr]);
  }

  getPoseFrame(index: number): NativeRecordPoseFrame {
    const poseFramePtr = this.wasmModule.ccall('rgbd_record_get_pose_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeRecordPoseFrame(this.wasmModule, poseFramePtr, false);
  }

  getCalibrationFrameCount(): number {
    return this.wasmModule.ccall('rgbd_record_get_calibration_frame_count', 'number', ['number'], [this.ptr]);
  }

  getCalibrationFrame(index: number): NativeRecordCalibrationFrame {
    const calibrationFramePtr = this.wasmModule.ccall('rgbd_record_get_calibration_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeRecordCalibrationFrame(this.wasmModule, calibrationFramePtr, false);
  }

  hasDirectionTable(): boolean {
    return this.wasmModule.ccall('rgbd_record_has_direction_table', 'boolean', ['number'], [this.ptr]);
  }

  getDirectionTable(): NativeDirectionTable {
    const directionTablePtr = this.wasmModule.ccall('rgbd_record_get_direction_table', 'number', ['number'], [this.ptr]);
    return new NativeDirectionTable(this.wasmModule, directionTablePtr, false);
  }
}
