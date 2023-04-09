import { CameraCalibration } from "./camera_calibration";
import { NativeByteArray } from "./capi_containers";
import { DepthCodecType } from "./depth_decoder";
import { NativeObject } from "./native_object";
import { RecordVideoFrame, RecordAudioFrame, RecordIMUFrame, RecordTRSFrame } from "./record";

export class NativeRecordBytesBuilder extends NativeObject {
  constructor(wasmModule: any) {
    const ptr = wasmModule.ccall('rgbd_record_bytes_builder_ctor', 'number', [], []);
    super(wasmModule, ptr, true);
  }

  delete() {
    this.wasmModule.ccall('rgbd_record_bytes_builder_dtor', null, ['number'], [this.ptr]);
  }

  setDepthCodecType(depthCodecType: DepthCodecType) {
    this.wasmModule.ccall('rgbd_record_bytes_builder_set_depth_codec_type', null, ['number', 'number'], [this.ptr, depthCodecType]);
  }

  setDepthUnit(depthUnit: number) {
    this.wasmModule.ccall('rgbd_record_bytes_builder_set_depth_unit', null, ['number', 'number'], [this.ptr, depthUnit]);
  }

  setCalibration(calibration: CameraCalibration) {
    const nativeCalibration = calibration.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_record_bytes_builder_set_calibration', null, ['number', 'number'], [this.ptr, nativeCalibration.ptr]);
    nativeCalibration.close();
  }

  setCoverPNGBytes(coverPNGBytes: Uint8Array) {
    const coverPNGBytesPtr = this.wasmModule._malloc(coverPNGBytes.byteLength);
    this.wasmModule.HEAPU8.set(coverPNGBytes, coverPNGBytesPtr);
    this.wasmModule.ccall('rgbd_record_bytes_builder_set_cover_png_bytes',
                          null,
                          ['number', 'number', 'number'],
                          [this.ptr, coverPNGBytesPtr, coverPNGBytes.byteLength]);
    this.wasmModule._free(coverPNGBytesPtr);
  }

  addVideoFrame(videoFrame: RecordVideoFrame) {
    const nativeVideoFrame = videoFrame.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_record_bytes_builder_add_video_frame', null, ['number', 'number'], [this.ptr, nativeVideoFrame.ptr]);
    nativeVideoFrame.close();
  }

  addAudioFrame(audioFrame: RecordAudioFrame) {
    const nativeAudioFrame = audioFrame.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_record_bytes_builder_add_audio_frame', null, ['number', 'number'], [this.ptr, nativeAudioFrame.ptr]);
    nativeAudioFrame.close();
  }

  addIMUFrame(imuFrame: RecordIMUFrame) {
    const nativeIMUFrame = imuFrame.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_record_bytes_builder_add_imu_frame', null, ['number', 'number'], [this.ptr, nativeIMUFrame.ptr]);
    nativeIMUFrame.close();
  }

  addTRSFrame(trsFrame: RecordTRSFrame) {
    const nativeTRSFrame = trsFrame.toNative(this.wasmModule);
    this.wasmModule.ccall('rgbd_record_bytes_builder_add_trs_frame', null, ['number', 'number'], [this.ptr, nativeTRSFrame.ptr]);
    nativeTRSFrame.close();
  }

  build(): Uint8Array {
    const nativeBytesPtr = this.wasmModule.ccall('rgbd_record_bytes_builder_build', null, ['number'], [this.ptr]);
    const nativeBytes = new NativeByteArray(this.wasmModule, nativeBytesPtr);
    const bytes = nativeBytes.toArray();
    nativeBytes.close();
    return bytes;
  }
}