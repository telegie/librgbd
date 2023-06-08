import { CameraCalibration } from "./camera_calibration";
import { NativeByteArray } from "./capi_containers";
import { DepthCodecType } from "./depth_decoder";
import { EmscriptenModuleEx } from "./emscripten_module_ex";
import { NativeObject } from "./native_object";
import { RecordVideoFrame, RecordAudioFrame, RecordIMUFrame, RecordPoseFrame } from "./record";

export class NativeRecordBytesBuilder extends NativeObject {
  constructor(module: EmscriptenModuleEx) {
    const ptr = module.ccall('rgbd_record_bytes_builder_ctor', 'number', [], []);
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_record_bytes_builder_dtor', null, ['number'], [this.getPtr()]);
  }

  setDepthCodecType(depthCodecType: DepthCodecType) {
    this.getModule().ccall('rgbd_record_bytes_builder_set_depth_codec_type', null, ['number', 'number'], [this.getPtr(), depthCodecType]);
  }

  setDepthUnit(depthUnit: number) {
    this.getModule().ccall('rgbd_record_bytes_builder_set_depth_unit', null, ['number', 'number'], [this.getPtr(), depthUnit]);
  }

  setCalibration(calibration: CameraCalibration) {
    const nativeCalibration = calibration.toNative(this.getModule());
    this.getModule().ccall('rgbd_record_bytes_builder_set_calibration', null, ['number', 'number'], [this.getPtr(), nativeCalibration.getPtr()]);
    nativeCalibration.close();
  }

  setCoverPNGBytes(coverPNGBytes: Uint8Array) {
    const coverPNGBytesPtr = this.getModule()._malloc(coverPNGBytes.byteLength);
    this.getModule().HEAPU8.set(coverPNGBytes, coverPNGBytesPtr);
    this.getModule().ccall('rgbd_record_bytes_builder_set_cover_png_bytes',
                          null,
                          ['number', 'number', 'number'],
                          [this.getPtr(), coverPNGBytesPtr, coverPNGBytes.byteLength]);
    this.getModule()._free(coverPNGBytesPtr);
  }

  addVideoFrame(videoFrame: RecordVideoFrame) {
    const nativeVideoFrame = videoFrame.toNative(this.getModule());
    this.getModule().ccall('rgbd_record_bytes_builder_add_video_frame', null, ['number', 'number'], [this.getPtr(), nativeVideoFrame.getPtr()]);
    nativeVideoFrame.close();
  }

  addAudioFrame(audioFrame: RecordAudioFrame) {
    const nativeAudioFrame = audioFrame.toNative(this.getModule());
    this.getModule().ccall('rgbd_record_bytes_builder_add_audio_frame', null, ['number', 'number'], [this.getPtr(), nativeAudioFrame.getPtr()]);
    nativeAudioFrame.close();
  }

  addIMUFrame(imuFrame: RecordIMUFrame) {
    const nativeIMUFrame = imuFrame.toNative(this.getModule());
    this.getModule().ccall('rgbd_record_bytes_builder_add_imu_frame', null, ['number', 'number'], [this.getPtr(), nativeIMUFrame.getPtr()]);
    nativeIMUFrame.close();
  }

  addPoseFrame(poseFrame: RecordPoseFrame) {
    const nativePoseFrame = poseFrame.toNative(this.getModule());
    this.getModule().ccall('rgbd_record_bytes_builder_add_pose_frame', null, ['number', 'number'], [this.getPtr(), nativePoseFrame.getPtr()]);
    nativePoseFrame.close();
  }

  build(): Uint8Array {
    const nativeBytesPtr = this.getModule().ccall('rgbd_record_bytes_builder_build', 'number', ['number'], [this.getPtr()]);
    const nativeBytes = new NativeByteArray(this.getModule(), nativeBytesPtr);
    const bytes = nativeBytes.toArray();
    nativeBytes.close();
    return bytes;
  }
}