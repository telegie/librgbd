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

  constructor(nativeInfo: NativeFileInfo) {
    this.timecodeScaleNs = nativeInfo.getTimecodeScaleNs();
    this.durationUs = nativeInfo.getDurationUs();
    this.writingApp = nativeInfo.getWritingApp();
  }
}

export class FileVideoTrack {
  trackNumber: number;
  width: number;
  height: number;

  constructor(nativeFileVideoTrack: NativeFileVideoTrack) {
    this.trackNumber = nativeFileVideoTrack.getTrackNumber();
    this.width = nativeFileVideoTrack.getWidth();
    this.height = nativeFileVideoTrack.getHeight();
  }
}

export class FileColorVideoTrack extends FileVideoTrack {
  codec: ColorCodecType;

  constructor(nativeFileColorVideoTrack: NativeFileColorVideoTrack) {
    super(nativeFileColorVideoTrack)
    this.codec = nativeFileColorVideoTrack.getCodec();
  }
}

export class FileDepthVideoTrack extends FileVideoTrack {
  codec: DepthCodecType;
  depthUnit: number;

  constructor(nativeFileDepthVideoTrack: NativeFileDepthVideoTrack) {
    super(nativeFileDepthVideoTrack)
    this.codec = nativeFileDepthVideoTrack.getCodec();
    this.depthUnit = nativeFileDepthVideoTrack.getDepthUnit();
  }
}

export class FileAudioTrack {
  trackNumber: number;
  samplingFrequency: number;

  constructor(nativeFileAudioTrack: NativeFileAudioTrack) {
    this.trackNumber = nativeFileAudioTrack.getTrackNumber();
    this.samplingFrequency = nativeFileAudioTrack.getSamplingFrequency();
  }
}

export class FileTracks {
  colorTrack: FileColorVideoTrack;
  depthTrack: FileDepthVideoTrack;
  audioTrack: FileAudioTrack;

  constructor(nativeFileTracks: NativeFileTracks) {
    const nativeColorTrack = nativeFileTracks.getColorTrack()
    this.colorTrack = new FileColorVideoTrack(nativeColorTrack);
    nativeColorTrack.close();

    const nativeDepthTrack = nativeFileTracks.getDepthTrack();
    this.depthTrack = new FileDepthVideoTrack(nativeDepthTrack);
    nativeDepthTrack.close();

    const nativeAudioTrack = nativeFileTracks.getAudioTrack();
    this.audioTrack = new FileAudioTrack(nativeAudioTrack);
    nativeAudioTrack.close();
  }
}

export class FileAttachments {
  calibration: CameraCalibration;
  coverPngBytes: Uint8Array | null;

  constructor(nativeFileAttachments: NativeFileAttachments) {
    const nativeCalibration = nativeFileAttachments.getCameraCalibration();
    this.calibration = CameraCalibration.create(nativeCalibration);
    nativeCalibration.close();

    this.coverPngBytes = nativeFileAttachments.getCoverPNGBytes();
  }
}

export class FileVideoFrame {
  timePointUs: number;
  keyframe: boolean;
  colorBytes: Uint8Array;
  depthBytes: Uint8Array;
  floor: Plane | null;

  constructor(nativeFileVideoFrame: NativeFileVideoFrame) {
    this.timePointUs = nativeFileVideoFrame.getTimePointUs();
    this.keyframe = nativeFileVideoFrame.getKeyframe();
    this.colorBytes = nativeFileVideoFrame.getColorBytes();
    this.depthBytes = nativeFileVideoFrame.getDepthBytes();
    this.floor = nativeFileVideoFrame.getFloor();
  }
}

export class FileAudioFrame {
  timePointUs: number;
  bytes: Uint8Array;

  constructor(nativeFileAudioFrame: NativeFileAudioFrame) {
    this.timePointUs = nativeFileAudioFrame.getTimePointUs();
    this.bytes = nativeFileAudioFrame.getBytes();
  }
}

export class FileIMUFrame {
  timePointUs: number;
  acceleration: Vector3;
  rotationRate: Vector3;
  magneticField: Vector3;
  gravity: Vector3;

  constructor(nativeFileIMUFrame: NativeFileIMUFrame) {
    this.timePointUs = nativeFileIMUFrame.getTimePointUs();
    this.acceleration = nativeFileIMUFrame.getAcceleration();
    this.rotationRate = nativeFileIMUFrame.getRotationRate();
    this.magneticField = nativeFileIMUFrame.getMagneticField();
    this.gravity = nativeFileIMUFrame.getGravity();
  }
}

export class FileTRSFrame {
  timePointUs: number;
  translation: Vector3;
  rotation: Quaternion;
  scale: Vector3;

  constructor(nativeFileTRSFrame: NativeFileTRSFrame) {
    this.timePointUs = nativeFileTRSFrame.getTimePointUs();
    this.translation = nativeFileTRSFrame.getTranslation();
    this.rotation = nativeFileTRSFrame.getRotation();
    this.scale = nativeFileTRSFrame.getScale();
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

  constructor(nativeFile: NativeFile) {
    const nativeInfo = nativeFile.getInfo();
    this.info = new FileInfo(nativeInfo);
    nativeInfo.close();

    const nativeTracks = nativeFile.getTracks();
    this.tracks = new FileTracks(nativeTracks);
    nativeTracks.close();

    const nativeAttachments = nativeFile.getAttachments();
    this.attachments = new FileAttachments(nativeAttachments);
    nativeAttachments.close();

    let videoFrames: FileVideoFrame[] = [];
    const videoFrameCount = nativeFile.getVideoFrameCount();
    for (let i = 0; i < videoFrameCount; i++) {
      const nativeVideoFrame = nativeFile.getVideoFrame(i);
      videoFrames.push(new FileVideoFrame(nativeVideoFrame));
      nativeVideoFrame.close();
    }
    this.videoFrames = videoFrames;

    let audioFrames: FileAudioFrame[] = [];
    const audioFrameCount = nativeFile.getAudioFrameCount();
    for (let i = 0; i < audioFrameCount; i++) {
      const nativeAudioFrame = nativeFile.getAudioFrame(i);
      audioFrames.push(new FileAudioFrame(nativeAudioFrame));
      nativeAudioFrame.close();
    }
    this.audioFrames = audioFrames;

    let imuFrames: FileIMUFrame[] = [];
    const imuFrameCount = nativeFile.getIMUFrameCount();
    for (let i = 0; i < imuFrameCount; i++) {
      const nativeIMUFrame = nativeFile.getIMUFrame(i);
      imuFrames.push(new FileIMUFrame(nativeIMUFrame));
      nativeIMUFrame.close();
    }
    this.imuFrames = imuFrames;

    let trsFrames: FileTRSFrame[] = [];
    const trsFrameCount = nativeFile.getTRSFrameCount();
    for (let i = 0; i < trsFrameCount; i++) {
      const nativeTRSFrame = nativeFile.getTRSFrame(i);
      trsFrames.push(new FileTRSFrame(nativeTRSFrame));
      nativeTRSFrame.close();
    }
    this.trsFrames = trsFrames;
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
    const codec: number = this.wasmModule.ccall('rgbd_file_color_video_track_get_codec', 'number', ['number'], [this.ptr]);
    if (codec === ColorCodecType.VP8)
      return ColorCodecType.VP8;
    throw new Error(`Invalid color codec found: ${codec}`);
  }
}

export class NativeFileDepthVideoTrack extends NativeFileVideoTrack {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  getCodec(): DepthCodecType {
    const codec: number = this.wasmModule.ccall('rgbd_file_depth_video_track_get_codec', 'number', ['number'], [this.ptr]);
    if (codec === DepthCodecType.RVL)
      return DepthCodecType.RVL;
    if (codec === DepthCodecType.TDC1)
      return DepthCodecType.TDC1;
    throw new Error(`Invalid depth codec found: ${codec}`);
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

  getFloor(): Plane | null {
    const hasFloor = this.wasmModule.ccall('rgbd_file_video_frame_has_floor', 'boolean', ['number'], [this.ptr]);
    if (!hasFloor) {
        return null;
    }
    const normalX = this.wasmModule.ccall('rgbd_file_video_frame_get_floor_normal_x', 'number', ['number'], [this.ptr]);
    const normalY = this.wasmModule.ccall('rgbd_file_video_frame_get_floor_normal_y', 'number', ['number'], [this.ptr]);
    const normalZ = this.wasmModule.ccall('rgbd_file_video_frame_get_floor_normal_z', 'number', ['number'], [this.ptr]);
    const constant = this.wasmModule.ccall('rgbd_file_video_frame_get_floor_constant', 'number', ['number'], [this.ptr]);

    return new Plane(new Vector3(normalX, normalY, normalZ), constant);
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
