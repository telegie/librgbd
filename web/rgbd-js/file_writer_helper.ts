import { CameraCalibration } from "./camera_calibration";
import { NativeByteArray } from "./capi_containers";
import { DepthCodecType } from "./depth_decoder";
import { FileVideoFrame, FileAudioFrame, FileIMUFrame, FileTRSFrame } from "./file";
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

  addAudioFrame(audioFrame: FileAudioFrame) {
    const nativeAudioFrame = audioFrame.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_file_writer_helper_add_audio_frame', null, ['number', 'number'], [this.ptr, nativeAudioFrame.ptr]);
    nativeAudioFrame.close();
  }

  addIMUFrame(imuFrame: FileIMUFrame) {
    const nativeIMUFrame = imuFrame.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_file_writer_helper_add_imu_frame', null, ['number', 'number'], [this.ptr, nativeIMUFrame.ptr]);
    nativeIMUFrame.close();
  }

  addTRSFrame(trsFrame: FileTRSFrame) {
    const nativeTRSFrame = trsFrame.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_file_writer_helper_add_trs_frame', null, ['number', 'number'], [this.ptr, nativeTRSFrame.ptr]);
    nativeTRSFrame.close();
  }

  writeToBytes(): Uint8Array {
    const nativeBytesPtr = this.wasmModule.ccall('rgbd_file_writer_helper_write_to_bytes', null, ['number'], [this.ptr]);
    const nativeBytes = new NativeByteArray(this.wasmModule, nativeBytesPtr);
    const bytes = nativeBytes.toArray();
    nativeBytes.close();
    return bytes;
  }
}