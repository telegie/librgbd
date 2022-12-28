import { CameraCalibration, NativeCameraCalibration } from './camera_calibration.js';
import { NativeByteArray, NativeString } from './capi_containers.js';
import { Plane } from './plane.js';
import { Vector3 } from './vector3.js';
import { Quaternion } from './quaternion.js';

export class FileInfo {
  constructor(nativeInfo) {
    this.timecodeScaleNs = nativeInfo.getTimecodeScaleNs();
    this.durationUs = nativeInfo.getDurationUs();
    this.writingApp = nativeInfo.getWritingApp();
  }
}

export class FileVideoTrack {
  constructor(nativeFileVideoTrack) {
    this.trackNumber = nativeFileVideoTrack.getTrackNumber();
    this.codec = nativeFileVideoTrack.getCodec();
    this.width = nativeFileVideoTrack.getWidth();
    this.height = nativeFileVideoTrack.getHeight();
  }
}

export class FileDepthVideoTrack extends FileVideoTrack {
  constructor(nativeFileDepthVideoTrack) {
    super(nativeFileDepthVideoTrack)
    this.depthUnit = nativeFileDepthVideoTrack.getDepthUnit();
  }
}

export class FileAudioTrack {
  constructor(nativeFileAudioTrack) {
    this.trackNumber = nativeFileAudioTrack.getTrackNumber();
    this.samplingFrequency = nativeFileAudioTrack.getSamplingFrequency();
  }
}

export class FileTracks {
  constructor(nativeFileTracks) {
    const nativeColorTrack = nativeFileTracks.getColorTrack()
    this.colorTrack = new FileVideoTrack(nativeColorTrack);
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
  constructor(nativeFileAttachments) {
    const nativeCalibration = nativeFileAttachments.getCameraCalibration();
    this.calibration = CameraCalibration.create(nativeCalibration);
    nativeCalibration.close();

    this.coverPngBytes = nativeFileAttachments.getCoverPNGBytes();
  }
}

export class FileVideoFrame {
  constructor(nativeFileVideoFrame) {
    this.globalTimecode = nativeFileVideoFrame.getGlobalTimecode();
    this.keyframe = nativeFileVideoFrame.getKeyframe();
    this.colorBytes = nativeFileVideoFrame.getColorBytes();
    this.depthBytes = nativeFileVideoFrame.getDepthBytes();
    this.floor = nativeFileVideoFrame.getFloor();
  }
}

export class FileAudioFrame {
  constructor(nativeFileAudioFrame) {
    this.globalTimecode = nativeFileAudioFrame.getGlobalTimecode();
    this.bytes = nativeFileAudioFrame.getBytes();
  }
}

export class FileIMUFrame {
  constructor(nativeFileIMUFrame) {
    this.globalTimecode = nativeFileIMUFrame.getGlobalTimecode();
    this.acceleration = nativeFileIMUFrame.getAcceleration();
    this.rotationRate = nativeFileIMUFrame.getRotationRate();
    this.magneticField = nativeFileIMUFrame.getMagneticField();
    this.gravity = nativeFileIMUFrame.getGravity();
  }
}

export class FileTRSFrame {
  constructor(nativeFileTRSFrame) {
    this.globalTimecode = nativeFileTRSFrame.getGlobalTimecode();
    this.translation = nativeFileTRSFrame.getTranslation();
    this.rotation = nativeFileTRSFrame.getRotation();
    this.scale = nativeFileTRSFrame.getScale();
  }
}

export class File {
  constructor(nativeFile) {
    const nativeInfo = nativeFile.getInfo();
    this.info = new FileInfo(nativeInfo);
    nativeInfo.close();

    const nativeTracks = nativeFile.getTracks();
    this.tracks = new FileTracks(nativeTracks);
    nativeTracks.close();

    const nativeAttachments = nativeFile.getAttachments();
    this.attachments = new FileAttachments(nativeAttachments);
    nativeAttachments.close();

    let videoFrames = [];
    const videoFrameCount = nativeFile.getVideoFrameCount();
    for (let i = 0; i < videoFrameCount; i++) {
      const nativeVideoFrame = nativeFile.getVideoFrame(i);
      videoFrames.push(new FileVideoFrame(nativeVideoFrame));
      nativeVideoFrame.close();
    }
    this.videoFrames = videoFrames;

    let audioFrames = [];
    const audioFrameCount = nativeFile.getAudioFrameCount();
    for (let i = 0; i < audioFrameCount; i++) {
      const nativeAudioFrame = nativeFile.getAudioFrame(i);
      audioFrames.push(new FileAudioFrame(nativeAudioFrame));
      nativeAudioFrame.close();
    }
    this.audioFrames = audioFrames;

    let imuFrames = [];
    const imuFrameCount = nativeFile.getIMUFrameCount();
    for (let i = 0; i < imuFrameCount; i++) {
      const nativeIMUFrame = nativeFile.getIMUFrame(i);
      imuFrames.push(new FileIMUFrame(nativeIMUFrame));
      nativeIMUFrame.close();
    }
    this.imuFrames = imuFrames;

    let trsFrames = [];
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
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_info_dtor', null, ['number'], [this.ptr]);
  }

  getTimecodeScaleNs() {
    return this.wasmModule.ccall('rgbd_file_info_get_timecode_scale_ns', 'number', ['number'], [this.ptr]);
  }

  getDurationUs() {
    return this.wasmModule.ccall('rgbd_file_info_get_duration_us', 'number', ['number'], [this.ptr]);
  }

  getWritingApp() {
    const nativeStrPtr = this.wasmModule.ccall('rgbd_file_info_get_writing_app', 'string', ['number'], [this.ptr]);
    const nativeStr = new NativeString(this.wasmModule, nativeStrPtr);
    const writingApp = nativeStr.toString();
    nativeStr.close();
    return writingApp;
  }
}

export class NativeFileVideoTrack {
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_video_track_dtor', 'null', ['number'], [this.ptr]);
  }

  getTrackNumber() {
    return this.wasmModule.ccall('rgbd_file_video_track_get_track_number', 'number', ['number'], [this.ptr]);
  }

  getCodec() {
    const nativeStrPtr = this.wasmModule.ccall('rgbd_file_video_track_get_codec', 'number', ['number'], [this.ptr]);
    const nativeStr = new NativeString(this.wasmModule, nativeStrPtr);
    const codec = nativeStr.toString();
    nativeStr.close();
    return codec;
  }

  getWidth() {
    return this.wasmModule.ccall('rgbd_file_video_track_get_width', 'number', ['number'], [this.ptr]);
  }

  getHeight() {
    return this.wasmModule.ccall('rgbd_file_video_track_get_height', 'number', ['number'], [this.ptr]);
  }
}

export class NativeFileDepthVideoTrack extends NativeFileVideoTrack {
  constructor(wasmModule, ptr, owner) {
    super(wasmModule, ptr, owner);
  }

  getDepthUnit() {
    return this.wasmModule.ccall('rgbd_file_depth_video_track_get_depth_unit', 'number', ['number'], [this.ptr]);
  }
}

export class NativeFileAudioTrack {
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_audio_track_dtor', 'null', ['number'], [this.ptr]);
  }

  getTrackNumber() {
    return this.wasmModule.ccall('rgbd_file_audio_track_get_track_number', 'number', ['number'], [this.ptr]);
  }

  getSamplingFrequency() {
    return this.wasmModule.ccall('rgbd_file_audio_track_get_sampling_frequency', 'number', ['number'], [this.ptr]);
  }
}

export class NativeFileTracks {
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_tracks_dtor', null, ['number'], [this.ptr]);
  }

  getColorTrack() {
    const trackPtr = this.wasmModule.ccall('rgbd_file_tracks_get_color_track', 'number', ['number'], [this.ptr]);
    return new NativeFileVideoTrack(this.wasmModule, trackPtr, false);
  }

  getDepthTrack() {
    const trackPtr = this.wasmModule.ccall('rgbd_file_tracks_get_depth_track', 'number', ['number'], [this.ptr]);
    return new NativeFileDepthVideoTrack(this.wasmModule, trackPtr, false);
  }

  getAudioTrack() {
    const trackPtr = this.wasmModule.ccall('rgbd_file_tracks_get_audio_track', 'number', ['number'], [this.ptr]);
    return new NativeFileAudioTrack(this.wasmModule, trackPtr, false);
  }
}

export class NativeFileAttachments {
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_attachments_dtor', null, ['number'], [this.ptr]);
  }

  getCameraCalibration() {
    const calibrationPtr = this.wasmModule.ccall('rgbd_file_attachments_get_camera_calibration', 'number', ['number'], [this.ptr]);
    return NativeCameraCalibration.create(this.wasmModule, calibrationPtr, false);
  }

  getCoverPNGBytes() {
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
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_video_frame_dtor', null, ['number'], [this.ptr]);
  }

  getGlobalTimecode() {
    return this.wasmModule.ccall('rgbd_file_video_frame_get_global_timecode', 'number', ['number'], [this.ptr]);
  }

  getKeyframe() {
    return this.wasmModule.ccall('rgbd_file_video_frame_get_keyframe', 'boolean', ['number'], [this.ptr]);
  }

  getColorBytes() {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_file_video_frame_get_color_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }

  getDepthBytes() {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_file_video_frame_get_depth_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }

  getFloor() {
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
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_audio_frame_dtor', null, ['number'], [this.ptr]);
  }

  getGlobalTimecode() {
    return this.wasmModule.ccall('rgbd_file_audio_frame_get_global_timecode', 'number', ['number'], [this.ptr]);
  }

  getBytes() {
    const nativeByteArrayPtr = this.wasmModule.ccall('rgbd_file_audio_frame_get_bytes', 'number', ['number'], [this.ptr]);
    const nativeByteArray = new NativeByteArray(this.wasmModule, nativeByteArrayPtr);
    const byteArray = nativeByteArray.toArray();
    nativeByteArray.close();

    return byteArray;
  }
}

export class NativeFileIMUFrame {
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_imu_frame_dtor', null, ['number'], [this.ptr]);
  }

  getGlobalTimecode() {
    return this.wasmModule.ccall('rgbd_file_imu_frame_get_global_timecode', 'number', ['number'], [this.ptr]);
  }

  getAcceleration() {
    const x  = this.wasmModule.ccall('rgbd_file_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_imu_frame_get_acceleration_x', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getRotationRate() {
    const x  = this.wasmModule.ccall('rgbd_file_imu_frame_get_rotation_rate_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_imu_frame_get_rotation_rate_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_imu_frame_get_rotation_rate_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getMagneticField() {
    const x  = this.wasmModule.ccall('rgbd_file_imu_frame_get_magnetic_field_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_imu_frame_get_magnetic_field_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_imu_frame_get_magnetic_field_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getGravity() {
    const x  = this.wasmModule.ccall('rgbd_file_imu_frame_get_gravity_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_imu_frame_get_gravity_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_imu_frame_get_gravity_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }
}

export class NativeFileTRSFrame {
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_file_trs_frame_dtor', null, ['number'], [this.ptr]);
  }

  getGlobalTimecode() {
    return this.wasmModule.ccall('rgbd_file_trs_frame_get_global_timecode', 'number', ['number'], [this.ptr]);
  }

  getTranslation() {
    const x  = this.wasmModule.ccall('rgbd_file_trs_frame_get_translation_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_trs_frame_get_translation_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_trs_frame_get_translation_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }

  getRotation() {
    const w  = this.wasmModule.ccall('rgbd_file_trs_frame_get_rotation_w', 'number', ['number'], [this.ptr]);
    const x  = this.wasmModule.ccall('rgbd_file_trs_frame_get_rotation_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_trs_frame_get_rotation_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_trs_frame_get_rotation_z', 'number', ['number'], [this.ptr]);
    return new Quaternion(w, x, y, z);
  }

  getScale() {
    const x  = this.wasmModule.ccall('rgbd_file_trs_frame_get_scale_x', 'number', ['number'], [this.ptr]);
    const y  = this.wasmModule.ccall('rgbd_file_trs_frame_get_scale_y', 'number', ['number'], [this.ptr]);
    const z  = this.wasmModule.ccall('rgbd_file_trs_frame_get_scale_z', 'number', ['number'], [this.ptr]);
    return new Vector3(x, y, z);
  }
}

export class NativeFile {
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_file_dtor', null, ['number'], [this.ptr]);
  }

  getInfo() {
    const infoPtr = this.wasmModule.ccall('rgbd_file_get_info', 'number', ['number'], [this.ptr]);
    return new NativeFileInfo(this.wasmModule, infoPtr, false);
  }

  getTracks() {
    const tracksPtr = this.wasmModule.ccall('rgbd_file_get_tracks', 'number', ['number'], [this.ptr]);
    return new NativeFileTracks(this.wasmModule, tracksPtr, false);
  }

  getAttachments() {
    const attachmentsPtr = this.wasmModule.ccall('rgbd_file_get_attachments', 'number', ['number'], [this.ptr]);
    return new NativeFileAttachments(this.wasmModule, attachmentsPtr, false);
  }

  getVideoFrameCount() {
    return this.wasmModule.ccall('rgbd_file_get_video_frame_count', 'number', ['number'], [this.ptr]);
  }

  getVideoFrame(index) {
    const videoFramePtr = this.wasmModule.ccall('rgbd_file_get_video_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeFileVideoFrame(this.wasmModule, videoFramePtr, false);
  }

  getAudioFrameCount() {
    return this.wasmModule.ccall('rgbd_file_get_audio_frame_count', 'number', ['number'], [this.ptr]);
  }

  getAudioFrame(index) {
    const audioFramePtr = this.wasmModule.ccall('rgbd_file_get_audio_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeFileAudioFrame(this.wasmModule, audioFramePtr, false);
  }

  getIMUFrameCount() {
    return this.wasmModule.ccall('rgbd_file_get_imu_frame_count', 'number', ['number'], [this.ptr]);
  }

  getIMUFrame(index) {
    const imuFramePtr = this.wasmModule.ccall('rgbd_file_get_imu_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeFileIMUFrame(this.wasmModule, imuFramePtr, false);
  }

  getTRSFrameCount() {
    return this.wasmModule.ccall('rgbd_file_get_trs_frame_count', 'number', ['number'], [this.ptr]);
  }

  getTRSFrame(index) {
    const imuFramePtr = this.wasmModule.ccall('rgbd_file_get_trs_frame', 'number', ['number', 'number'], [this.ptr, index]);
    return new NativeFileTRSFrame(this.wasmModule, imuFramePtr, false);
  }
}
