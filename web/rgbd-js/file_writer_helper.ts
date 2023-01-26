import { CameraCalibration } from "./camera_calibration";
import { DepthCodecType } from "./depth_decoder";
import { FileVideoFrame, FileAudioFrame, FileIMUFrame, FileTRSFrame } from "./file";
import { NativeFileWriter, NativeFileWriterConfig } from "./file_writer";
import { YuvFrame } from "./yuv_frame";

export class NativeFileWriterHelper {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any) {
    this.wasmModule = wasmModule;
    this.ptr = this.wasmModule.ccall('rgbd_file_writer_helper_ctor', 'number', [], []);
  }

  close() {
    this.wasmModule.ccall('rgbd_file_writer_helper_dtor', null, ['number'], [this.ptr]);
  }

  setCalibration(calibration: CameraCalibration) {
    const nativeCalibration = calibration.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_file_writer_helper_set_calibration', null, ['number', 'number'], [this.ptr, nativeCalibration.ptr]);
    nativeCalibration.close();
  }

  setDepthCodecType(depthCodecType: DepthCodecType) {
    this.wasmModule.ccall('rgbd_file_writer_helper_set_depth_codec_type', null, ['number', 'number'], [this.ptr, depthCodecType]);
  }

  setDepthUnit(depthUnit: number) {
    this.wasmModule.ccall('rgbd_file_writer_helper_set_depth_unit', null, ['number', 'number'], [this.ptr, depthUnit]);
  }

  setCover(cover: YuvFrame) {
    const nativeCover = cover.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_file_writer_helper_set_cover', null, ['number', 'number'], [this.ptr, nativeCover.ptr]);
    nativeCover.close();
  }

  addVideoFrame(videoFrame: FileVideoFrame) {
    const nativeVideoFrame = videoFrame.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_file_writer_helper_add_video_frame', null, ['number', 'number'], [this.ptr, nativeVideoFrame.ptr]);
    nativeVideoFrame.close();
  }
}

export class FileWriterHelper {
  wasmModule: any;
  calibration: CameraCalibration | null;
  depthCodecType: DepthCodecType;
  depthUnit: number | null;
  cover: YuvFrame | null;
  videoFrames: FileVideoFrame[];
  audioFrames: FileAudioFrame[];
  imuFrames: FileIMUFrame[];
  trsFrames: FileTRSFrame[];

  constructor(wasmModule: any) {
    this.wasmModule = wasmModule;
    this.calibration = null;
    this.depthCodecType = DepthCodecType.TDC1;
    this.depthUnit = null;
    this.cover = null;
    this.videoFrames = [];
    this.audioFrames = [];
    this.imuFrames = [];
    this.trsFrames = [];
  }

  setCalibration(calibration: CameraCalibration) {
    this.calibration = calibration;
  }

  setDepthCodecType(depthCodecType: DepthCodecType) {
    this.depthCodecType = depthCodecType;
  }

  setDepthUnit(depthUnit: number) {
    this.depthUnit = depthUnit;
  }

  setCover(cover: YuvFrame) {
    this.cover = cover;
  }

  addVideoFrame(videoFrame: FileVideoFrame) {
    this.videoFrames.push(videoFrame);
  }

  addAudioFrame(audioFrame: FileAudioFrame) {
    this.audioFrames.push(audioFrame);
  }

  addIMUFrame(imuFrame: FileIMUFrame) {
    this.imuFrames.push(imuFrame);
  }

  addTRSFrame(trsFrame: FileTRSFrame) {
    this.trsFrames.push(trsFrame);
  }

  write(): Uint8Array {
    this.videoFrames.sort((a, b) => a.timePointUs - b.timePointUs);
    this.audioFrames.sort((a, b) => a.timePointUs - b.timePointUs);
    this.imuFrames.sort((a, b) => a.timePointUs - b.timePointUs);
    this.trsFrames.sort((a, b) => a.timePointUs - b.timePointUs);

    let initialTimePoints: number[] = [];
    if (this.videoFrames.length > 0)
      initialTimePoints.push(this.videoFrames[0].timePointUs);
    if (this.audioFrames.length > 0)
      initialTimePoints.push(this.audioFrames[0].timePointUs);
    if (this.imuFrames.length > 0)
      initialTimePoints.push(this.imuFrames[0].timePointUs);
    if (this.trsFrames.length > 0)
      initialTimePoints.push(this.trsFrames[0].timePointUs);
    const minimumTimePointsUs = Math.min(...initialTimePoints);
  
    if (!this.calibration)
      throw new Error("No CameraCalibration found from FileWriterHelper");

    let writerConfig = new NativeFileWriterConfig(this.wasmModule);
    writerConfig.setDepthCodecType(this.depthCodecType);
    if (this.depthUnit !== null) {
      writerConfig.setDepthUnit(this.depthUnit);
    }
    const fileWriter = new NativeFileWriter(this.wasmModule, this.calibration, writerConfig);

    if (this.cover)
      fileWriter.writeCover(this.cover);
    
    let audioFrameIndex = 0;
    let imuFrameIndex = 0;
    let trsFrameIndex = 0;
    for (const videoFrame of this.videoFrames) {
      const videoTimePointUs = videoFrame.timePointUs;

      while (audioFrameIndex < this.audioFrames.length) {
        const audioFrame = this.audioFrames[audioFrameIndex];
        if (audioFrame.timePointUs > videoTimePointUs)
          break;

        fileWriter.writeAudioFrame(audioFrame.timePointUs - minimumTimePointsUs,
                                   audioFrame.bytes);
        audioFrameIndex += 1;
      }

      while (imuFrameIndex < this.imuFrames.length) {
        const imuFrame = this.imuFrames[imuFrameIndex];
        if (imuFrame.timePointUs > videoTimePointUs)
          break;
        
        fileWriter.writeIMUFrame(imuFrame.timePointUs - minimumTimePointsUs,
                                 imuFrame.acceleration,
                                 imuFrame.rotationRate,
                                 imuFrame.magneticField,
                                 imuFrame.gravity);
        imuFrameIndex += 1;
      }

      while (trsFrameIndex < this.trsFrames.length) {
        const trsFrame = this.trsFrames[trsFrameIndex];
        if (trsFrame.timePointUs > videoTimePointUs)
          break;
        
        fileWriter.writeTRSFrame(trsFrame.timePointUs - minimumTimePointsUs,
                                 trsFrame.translation,
                                 trsFrame.rotation,
                                 trsFrame.scale);
        trsFrameIndex += 1;
      }

      fileWriter.writeVideoFrame(videoFrame.timePointUs - minimumTimePointsUs,
                                 videoFrame.keyframe,
                                 videoFrame.colorBytes,
                                 videoFrame.depthBytes);
    }
    fileWriter.flush();
    const bytes = fileWriter.getBytes();
    fileWriter.close();
    return bytes;
  }
}