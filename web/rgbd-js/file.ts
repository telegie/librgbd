import { CameraCalibration, NativeCameraCalibration } from './camera_calibration';
import { NativeByteArray, NativeString } from './capi_containers';
import { Plane } from './plane';
import { Vector3 } from './vector3';
import { Quaternion } from './quaternion';
import { DepthCodecType } from './depth_decoder';
import { ColorCodecType } from './color_decoder';

export class FileInfo {
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

  static fromNative(nativeInfo: NativeFileInfo) {
    const timecodeScaleNs = nativeInfo.getTimecodeScaleNs();
    const durationUs = nativeInfo.getDurationUs();
    const writingApp = nativeInfo.getWritingApp();
    return new FileInfo(timecodeScaleNs, durationUs, writingApp);
  }
}

export class FileVideoTrack {
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

  static fromNative(nativeFileVideoTrack: NativeFileVideoTrack) {
    const trackNumber = nativeFileVideoTrack.getTrackNumber();
    const width = nativeFileVideoTrack.getWidth();
    const height = nativeFileVideoTrack.getHeight();
    return new FileVideoTrack(trackNumber, width, height);
  }
}

export class FileColorVideoTrack extends FileVideoTrack {
  codec: ColorCodecType;

  constructor(trackNumber: number,
              width: number,
              height: number,
              codec: ColorCodecType) {
    super(trackNumber, width, height);
    this.codec = codec;
  }

  static fromNative(nativeFileColorVideoTrack: NativeFileColorVideoTrack) {
    const trackNumber = nativeFileColorVideoTrack.getTrackNumber();
    const width = nativeFileColorVideoTrack.getWidth();
    const height = nativeFileColorVideoTrack.getHeight();
    const codec = nativeFileColorVideoTrack.getCodec();
    return new FileColorVideoTrack(trackNumber, width, height, codec);
  }
}

export class FileDepthVideoTrack extends FileVideoTrack {
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

  static fromNative(nativeFileDepthVideoTrack: NativeFileDepthVideoTrack) {
    const trackNumber = nativeFileDepthVideoTrack.getTrackNumber();
    const width = nativeFileDepthVideoTrack.getWidth();
    const height = nativeFileDepthVideoTrack.getHeight();
    const codec = nativeFileDepthVideoTrack.getCodec();
    const depthUnit = nativeFileDepthVideoTrack.getDepthUnit();
    return new FileDepthVideoTrack(trackNumber, width, height, codec, depthUnit);
  }
}

export class FileAudioTrack {
  trackNumber: number;
  samplingFrequency: number;

  constructor(trackNumber: number,
              samplingFrequency: number) {
    this.trackNumber = trackNumber;
    this.samplingFrequency = samplingFrequency;
  }

  static fromNative(nativeFileAudioTrack: NativeFileAudioTrack) {
    const trackNumber = nativeFileAudioTrack.getTrackNumber();
    const samplingFrequency = nativeFileAudioTrack.getSamplingFrequency();
    return new FileAudioTrack(trackNumber, samplingFrequency);
  }
}

export class FileTracks {
  colorTrack: FileColorVideoTrack;
  depthTrack: FileDepthVideoTrack;
  audioTrack: FileAudioTrack;

  constructor(colorTrack: FileColorVideoTrack,
              depthTrack: FileDepthVideoTrack,
              audioTrack: FileAudioTrack) {
    this.colorTrack = colorTrack;
    this.depthTrack = depthTrack;
    this.audioTrack = audioTrack;
  }

  static fromNative(nativeFileTracks: NativeFileTracks) {
    const nativeColorTrack = nativeFileTracks.getColorTrack()
    const colorTrack = FileColorVideoTrack.fromNative(nativeColorTrack);
    nativeColorTrack.close();

    const nativeDepthTrack = nativeFileTracks.getDepthTrack();
    const depthTrack = FileDepthVideoTrack.fromNative(nativeDepthTrack);
    nativeDepthTrack.close();

    const nativeAudioTrack = nativeFileTracks.getAudioTrack();
    const audioTrack = FileAudioTrack.fromNative(nativeAudioTrack);
    nativeAudioTrack.close();

    return new FileTracks(colorTrack, depthTrack, audioTrack);
  }
}

export class FileAttachments {
  calibration: CameraCalibration;
  coverPngBytes: Uint8Array | null;

  constructor(calibration: CameraCalibration,
              coverPngBytes: Uint8Array | null) {
    this.calibration = calibration;
    this.coverPngBytes = coverPngBytes;
  }

  static fromNative(nativeFileAttachments: NativeFileAttachments) {
    const nativeCalibration = nativeFileAttachments.getCameraCalibration();
    const calibration = CameraCalibration.fromNative(nativeCalibration);
    nativeCalibration.close();

    const coverPngBytes = nativeFileAttachments.getCoverPNGBytes();
    return new FileAttachments(calibration, coverPngBytes);
  }
}

export class FileVideoFrame {
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

  static fromNative(nativeFileVideoFrame: NativeFileVideoFrame) {
    const timePointUs = nativeFileVideoFrame.getTimePointUs();
    const keyframe = nativeFileVideoFrame.getKeyframe();
    const colorBytes = nativeFileVideoFrame.getColorBytes();
    const depthBytes = nativeFileVideoFrame.getDepthBytes();
    return new FileVideoFrame(timePointUs, keyframe, colorBytes, depthBytes);
  }

  toNative(wasmModule: any) {
    const colorBytesPtr = wasmModule._malloc(this.colorBytes.byteLength);
    const depthBytesPtr = wasmModule._malloc(this.depthBytes.byteLength);
    wasmModule.HEAPU8.set(this.colorBytes, colorBytesPtr);
    wasmModule.HEAPU8.set(this.depthBytes, depthBytesPtr);

    const ptr = wasmModule.ccall('rgbd_file_video_frame_ctor_wasm',
                                 'number',
                                 ['number', 'number', 'number', 'number', 'number', 'number'],
                                 [this.timePointUs, this.keyframe,
                                  colorBytesPtr, this.colorBytes.byteLength,
                                  depthBytesPtr, this.depthBytes.byteLength]);
    wasmModule._free(colorBytesPtr);
    wasmModule._free(depthBytesPtr);

    return new NativeFileVideoFrame(wasmModule, ptr, true);
  }
}

export class FileAudioFrame {
  timePointUs: number;
  bytes: Uint8Array;

  constructor(timePointUs: number, bytes: Uint8Array) {
    this.timePointUs = timePointUs;
    this.bytes = bytes;
  }

  static fromNative(nativeFileAudioFrame: NativeFileAudioFrame) {
    const timePointUs = nativeFileAudioFrame.getTimePointUs();
    const bytes = nativeFileAudioFrame.getBytes();
    return new FileAudioFrame(timePointUs, bytes);
  }
}

export class FileIMUFrame {
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

  static fromNative(nativeFileIMUFrame: NativeFileIMUFrame) {
    const timePointUs = nativeFileIMUFrame.getTimePointUs();
    const acceleration = nativeFileIMUFrame.getAcceleration();
    const rotationRate = nativeFileIMUFrame.getRotationRate();
    const magneticField = nativeFileIMUFrame.getMagneticField();
    const gravity = nativeFileIMUFrame.getGravity();
    return new FileIMUFrame(timePointUs,
                            acceleration,
                            rotationRate,
                            magneticField,
                            gravity)
  }
}

export class FileTRSFrame {
  timePointUs: number;
  translation: Vector3;
  rotation: Quaternion;
  scale: Vector3;

  constructor(timePointUs: number,
              translation: Vector3,
              rotation: Quaternion,
              scale: Vector3) {
    this.timePointUs = timePointUs;
    this.translation = translation;
    this.rotation = rotation;
    this.scale = scale;
  }

  static fromNative(nativeFileTRSFrame: NativeFileTRSFrame) {
    const timePointUs = nativeFileTRSFrame.getTimePointUs();
    const translation = nativeFileTRSFrame.getTranslation();
    const rotation = nativeFileTRSFrame.getRotation();
    const scale = nativeFileTRSFrame.getScale();
    return new FileTRSFrame(timePointUs, translation, rotation, scale);
  }
}

export class File {
  info: FileInfo;
  tracks: FileTracks;
  attachments: FileAttachments;
  videoFrames: FileVideoFrame[];
  audioFrames: FileAudioFrame[];
  imuFrames: FileIMUFrame[];
  trsFrames: FileTRSFrame[];

  constructor(info: FileInfo,
              tracks: FileTracks,
              attachments: FileAttachments,
              videoFrames: FileVideoFrame[],
              audioFrames: FileAudioFrame[],
              imuFrames: FileIMUFrame[],
              trsFrames: FileTRSFrame[]) {
    this.info = info;
    this.tracks = tracks;
    this.attachments = attachments;
    this.videoFrames = videoFrames;
    this.audioFrames = audioFrames;
    this.imuFrames = imuFrames;
    this.trsFrames = trsFrames;
  }

  static fromNative(nativeFile: NativeFile) {
    const nativeInfo = nativeFile.getInfo();
    const info = FileInfo.fromNative(nativeInfo);
    nativeInfo.close();

    const nativeTracks = nativeFile.getTracks();
    const tracks = FileTracks.fromNative(nativeTracks);
    nativeTracks.close();

    const nativeAttachments = nativeFile.getAttachments();
    const attachments = FileAttachments.fromNative(nativeAttachments);
    nativeAttachments.close();

    let videoFrames: FileVideoFrame[] = [];
    const videoFrameCount = nativeFile.getVideoFrameCount();
    for (let i = 0; i < videoFrameCount; i++) {
      const nativeVideoFrame = nativeFile.getVideoFrame(i);
      videoFrames.push(FileVideoFrame.fromNative(nativeVideoFrame));
      nativeVideoFrame.close();
    }

    let audioFrames: FileAudioFrame[] = [];
    const audioFrameCount = nativeFile.getAudioFrameCount();
    for (let i = 0; i < audioFrameCount; i++) {
      const nativeAudioFrame = nativeFile.getAudioFrame(i);
      audioFrames.push(FileAudioFrame.fromNative(nativeAudioFrame));
      nativeAudioFrame.close();
    }

    let imuFrames: FileIMUFrame[] = [];
    const imuFrameCount = nativeFile.getIMUFrameCount();
    for (let i = 0; i < imuFrameCount; i++) {
      const nativeIMUFrame = nativeFile.getIMUFrame(i);
      imuFrames.push(FileIMUFrame.fromNative(nativeIMUFrame));
      nativeIMUFrame.close();
    }

    let trsFrames: FileTRSFrame[] = [];
    const trsFrameCount = nativeFile.getTRSFrameCount();
    for (let i = 0; i < trsFrameCount; i++) {
      const nativeTRSFrame = nativeFile.getTRSFrame(i);
      trsFrames.push(FileTRSFrame.fromNative(nativeTRSFrame));
      nativeTRSFrame.close();
    }
    
    return new File(info, tracks, attachments,
      videoFrames, audioFrames, imuFrames, trsFrames);
  }
}

export class NativeFileInfo {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_info_dtor', null, ['number'], [this.ptr]);
  }

  getTimecodeScaleNs(): number {
    return this.wasmModule.ccall('rgbd_file_info_get_timecode_scale_ns', 'number', ['number'], [this.ptr]);
  }

  getDurationUs(): number {
    return this.wasmModule.ccall('rgbd_file_info_get_duration_us', 'number', ['number'], [this.ptr]);
  }

  getWritingApp(): string {
    const nativeStrPtr = this.wasmModule.ccall('rgbd_file_info_get_writing_app', 'string', ['number'], [this.ptr]);
    const nativeStr = new NativeString(this.wasmModule, nativeStrPtr);
    const writingApp = nativeStr.toString();
    nativeStr.close();
    return writingApp;
  }
}

export class NativeFileVideoTrack {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_video_track_dtor', 'null', ['number'], [this.ptr]);
  }

  getTrackNumber(): number {
    return this.wasmModule.ccall('rgbd_file_video_track_get_track_number', 'number', ['number'], [this.ptr]);
  }

  getWidth(): number {
    return this.wasmModule.ccall('rgbd_file_video_track_get_width', 'number', ['number'], [this.ptr]);
  }

  getHeight(): number {
    return this.wasmModule.ccall('rgbd_file_video_track_get_height', 'number', ['number'], [this.ptr]);
  }
}

export class NativeFileColorVideoTrack extends NativeFileVideoTrack {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  getCodec(): ColorCodecType {
    return this.wasmModule.ccall('rgbd_file_color_video_track_get_codec', 'number', ['number'], [this.ptr]);
  }
}

export class NativeFileDepthVideoTrack extends NativeFileVideoTrack {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  getCodec(): DepthCodecType {
    return this.wasmModule.ccall('rgbd_file_depth_video_track_get_codec', 'number', ['number'], [this.ptr]);
  }

  getDepthUnit(): number {
    return this.wasmModule.ccall('rgbd_file_depth_video_track_get_depth_unit', 'number', ['number'], [this.ptr]);
  }
}

export class NativeFileAudioTrack {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_audio_track_dtor', 'null', ['number'], [this.ptr]);
  }

  getTrackNumber(): number {
    return this.wasmModule.ccall('rgbd_file_audio_track_get_track_number', 'number', ['number'], [this.ptr]);
  }

  getSamplingFrequency(): number {
    return this.wasmModule.ccall('rgbd_file_audio_track_get_sampling_frequency', 'number', ['number'], [this.ptr]);
  }
}

export class NativeFileTracks {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_tracks_dtor', null, ['number'], [this.ptr]);
  }

  getColorTrack(): NativeFileColorVideoTrack {
    const trackPtr = this.wasmModule.ccall('rgbd_file_tracks_get_color_track', 'number', ['number'], [this.ptr]);
    return new NativeFileColorVideoTrack(this.wasmModule, trackPtr, false);
  }

  getDepthTrack(): NativeFileDepthVideoTrack {
    const trackPtr = this.wasmModule.ccall('rgbd_file_tracks_get_depth_track', 'number', ['number'], [this.ptr]);
    return new NativeFileDepthVideoTrack(this.wasmModule, trackPtr, false);
  }

  getAudioTrack(): NativeFileAudioTrack {
    const trackPtr = this.wasmModule.ccall('rgbd_file_tracks_get_audio_track', 'number', ['number'], [this.ptr]);
    return new NativeFileAudioTrack(this.wasmModule, trackPtr, false);
  }
}

export class NativeFileAttachments {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_attachments_dtor', null, ['number'], [this.ptr]);
  }

  getCameraCalibration(): NativeCameraCalibration {
    const calibrationPtr = this.wasmModule.ccall('rgbd_file_attachments_get_camera_calibration', 'number', ['number'], [this.ptr]);
    return NativeCameraCalibration.create(this.wasmModule, calibrationPtr, false);
  }

  getCoverPNGBytes(): Uint8Array | null {
    const bytesPtr = this.wasmModule.ccall('rgbd_file_attachments_get_cover_png_bytes', 'number', ['number'], [this.ptr]);
    if (bytesPtr == 0)
      return null;

    const nativeByteArray = new NativeByteArray(this.wasmModule, bytesPtr);
    const bytes = nativeByteArray.toArray();
    nativeByteArray.close();
    return bytes;
  }
}

export class NativeFileVideoFrame {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_video_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_file_video_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getKeyframe(): boolean {
    return this.wasmModule.ccall('rgbd_file_video_frame_get_keyframe', 'boolean', ['number'], [this.ptr]);
  }

  getColorBytes(): Uint8Array {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_file_video_frame_get_color_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }

  getDepthBytes(): Uint8Array {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_file_video_frame_get_depth_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }
}

export class NativeFileAudioFrame {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_audio_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_file_audio_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getBytes(): Uint8Array {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_file_audio_frame_get_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }
}

export class NativeFileIMUFrame {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_imu_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_file_imu_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getAcceleration(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_file_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getRotationRate(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_file_imu_frame_get_rotation_rate_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_imu_frame_get_rotation_rate_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_imu_frame_get_rotation_rate_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getMagneticField(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_file_imu_frame_get_magnetic_field_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_imu_frame_get_magnetic_field_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_imu_frame_get_magnetic_field_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getGravity(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_file_imu_frame_get_gravity_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_imu_frame_get_gravity_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_imu_frame_get_gravity_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }
}

export class NativeFileTRSFrame {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_trs_frame_dtor', null, ['number'], [this.ptr]);
  }

  getTimePointUs(): number {
    return this.wasmModule.ccall('rgbd_file_trs_frame_get_time_point_us', 'number', ['number'], [this.ptr]);
  }

  getTranslation(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_file_trs_frame_get_translation_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_trs_frame_get_translation_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_trs_frame_get_translation_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getRotation(): Quaternion {
    const w  = this.wasmModule.ccall('rgbd_file_trs_frame_get_rotation_w', 'number', ['number'], [this.ptr]);
    const x  = this.wasmModule.ccall('rgbd_file_trs_frame_get_rotation_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_trs_frame_get_rotation_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_trs_frame_get_rotation_z', 'number', ['number'], [this.ptr]);
    return new Quaternion(w, x, y, z);
  }

  getScale(): Vector3 {
    const x  = this.wasmModule.ccall('rgbd_file_trs_frame_get_scale_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_trs_frame_get_scale_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_trs_frame_get_scale_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }
}

export class NativeFile {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_file_dtor', null, ['number'], [this.ptr]);
  }

  getInfo(): NativeFileInfo {
    const infoPtr = this.wasmModule.ccall('rgbd_file_get_info', 'number', ['number'], [this.ptr]);
    return new NativeFileInfo(this.wasmModule, infoPtr, false);
  }

  getTracks(): NativeFileTracks {
    const tracksPtr = this.wasmModule.ccall('rgbd_file_get_tracks', 'number', ['number'], [this.ptr]);
    return new NativeFileTracks(this.wasmModule, tracksPtr, false);
  }

  getAttachments(): NativeFileAttachments {
    const attachmentsPtr = this.wasmModule.ccall('rgbd_file_get_attachments', 'number', ['number'], [this.ptr]);
    return new NativeFileAttachments(this.wasmModule, attachmentsPtr, false);
  }

  getVideoFrameCount(): number {
    return this.wasmModule.ccall('rgbd_file_get_video_frame_count', 'number', ['number'], [this.ptr]);
  }

  getVideoFrame(index: number): NativeFileVideoFrame {
    const videoFramePtr = this.wasmModule.ccall('rgbd_file_get_video_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeFileVideoFrame(this.wasmModule, videoFramePtr, false);
  }

  getAudioFrameCount(): number {
    return this.wasmModule.ccall('rgbd_file_get_audio_frame_count', 'number', ['number'], [this.ptr]);
  }

  getAudioFrame(index: number): NativeFileAudioFrame {
    const audioFramePtr = this.wasmModule.ccall('rgbd_file_get_audio_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeFileAudioFrame(this.wasmModule, audioFramePtr, false);
  }

  getIMUFrameCount(): number {
    return this.wasmModule.ccall('rgbd_file_get_imu_frame_count', 'number', ['number'], [this.ptr]);
  }

  getIMUFrame(index: number): NativeFileIMUFrame {
    const imuFramePtr = this.wasmModule.ccall('rgbd_file_get_imu_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeFileIMUFrame(this.wasmModule, imuFramePtr, false);
  }

  getTRSFrameCount(): number {
    return this.wasmModule.ccall('rgbd_file_get_trs_frame_count', 'number', ['number'], [this.ptr]);
  }

  getTRSFrame(index: number): NativeFileTRSFrame {
    const imuFramePtr = this.wasmModule.ccall('rgbd_file_get_trs_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeFileTRSFrame(this.wasmModule, imuFramePtr, false);
  }
}
