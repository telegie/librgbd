import { NativeByteArray } from './capi_containers.js';

export class NativeFileWriterConfig {
  constructor(wasmModule) {
    this.wasmModule = wasmModule;
    this.ptr = this.wasmModule.ccall('rgbd_file_writer_config_ctor', 'number', [], []);
  }

  close() {
    this.wasmModule.ccall('rgbd_file_writer_config_dtor', null, ['number'], [this.ptr]);
  }

  setFramerate(framerate) {
    this.wasmModule.ccall('rgbd_file_writer_config_set_framerate',
                          null,
                          ['number', 'number'],
                          [this.ptr, framerate]);
  }

  setSamplerate(samplerate) {
    this.wasmModule.ccall('rgbd_file_writer_config_set_samplerate',
                          null,
                          ['number', 'number'],
                          [this.ptr, samplerate]);
  }

  setDepthCodecType(depthCodecType) {
    this.wasmModule.ccall('rgbd_file_writer_config_set_depth_codec_type',
                          null,
                          ['number', 'number'],
                          [this.ptr, depthCodecType]);
  }

  getDepthUnit() {
    return this.wasmModule.ccall('rgbd_file_writer_config_get_depth_unit',
                                 'number',
                                 ['number'],
                                 [this.ptr]);
  }

  setDepthUnit(depthUnit) {
    this.wasmModule.ccall('rgbd_file_writer_config_set_depth_unit',
                          null,
                          ['number', 'number'],
                          [this.ptr, depthUnit]);
  }
}

export class NativeFileWriter {
  constructor(wasmModule, calibration, nativeWriterConfig) {
    this.wasmModule = wasmModule;

    const nativeCalibration = calibration.createNativeInstance();
    this.ptr = this.wasmModule.ccall('rgbd_file_writer_ctor_in_memory',
                                     'number',
                                     ['number', 'number'],
                                     [nativeCalibration.ptr, nativeWriterConfig.ptr]);
    nativeCalibration.close();
  }

  close() {
    this.wasmModule.ccall('rgbd_file_writer_dtor', null, ['number'], [this.ptr]);
  }

  writeCover(yuvFrame) {
    const yChannelPtr = this.wasmModule._malloc(yuvFrame.yChannel.byteLength);
    const uChannelPtr = this.wasmModule._malloc(yuvFrame.uChannel.byteLength);
    const vChannelPtr = this.wasmModule._malloc(yuvFrame.vChannel.byteLength);
    this.wasmModule.HEAPU8.set(yuvFrame.yChannel, yChannelPtr);
    this.wasmModule.HEAPU8.set(yuvFrame.uChannel, uChannelPtr);
    this.wasmModule.HEAPU8.set(yuvFrame.vChannel, vChannelPtr);
    this.wasmModule.ccall('rgbd_file_writer_write_cover',
                          null,
                          ['number', 'number', 'number', 'number', 'number', 'number'],
                          [this.ptr, yuvFrame.width, yuvFrame.height, yChannelPtr, uChannelPtr, vChannelPtr]);
    this.wasmModule._free(yChannelPtr);
    this.wasmModule._free(uChannelPtr);
    this.wasmModule._free(vChannelPtr);
  }

  writeVideoFrame(timePointUs, keyframe, colorBytes, depthBytes) {
    const colorBytesPtr = this.wasmModule._malloc(colorBytes.byteLength);
    const depthBytesPtr = this.wasmModule._malloc(depthBytes.byteLength);
    this.wasmModule.HEAPU8.set(colorBytes, colorBytesPtr);
    this.wasmModule.HEAPU8.set(depthBytes, depthBytesPtr);
    this.wasmModule.ccall('rgbd_file_writer_write_video_frame_wasm',
                          null,
                          ['number', 'number', 'boolean',
                           'number', 'number',
                           'number', 'number'],
                          [this.ptr, timePointUs, keyframe,
                           colorBytesPtr, colorBytes.byteLength,
                           depthBytesPtr, depthBytes.byteLength]);
    this.wasmModule._free(colorBytesPtr);
    this.wasmModule._free(depthBytesPtr);
  }

  writeAudioFrame(timePointUs, audioBytes) {
    const audioBytesPtr = this.wasmModule._malloc(audioBytes.byteLength);
    this.wasmModule.HEAPU8.set(audioBytes, audioBytesPtr);
    this.wasmModule.ccall('rgbd_file_writer_write_audio_frame_wasm',
                          null,
                          ['number', 'number', 'number', 'number'],
                          [this.ptr, timePointUs, audioBytesPtr, audioBytes.byteLength]);
    this.wasmModule._free(audioBytesPtr);
  }

  writeIMUFrame(timePointUs, acceleration, rotationRate, magneticField, gravity) {
    this.wasmModule.ccall('rgbd_file_writer_write_imu_frame_wasm',
                          null,
                          ['number', 'number',
                           'number', 'number', 'number',
                           'number', 'number', 'number',
                           'number', 'number', 'number',
                           'number', 'number', 'number'],
                          [this.ptr, timePointUs,
                           acceleration.x, acceleration.y, acceleration.z,
                           rotationRate.x, rotationRate.y, rotationRate.z,
                           magneticField.x, magneticField.y, magneticField.z,
                           gravity.x, gravity.y, gravity.z]);
  }

  writeTRSFrame(timePointUs, translation, rotation, scale) {
    this.wasmModule.ccall('rgbd_file_writer_write_trs_frame_wasm',
                          null,
                          ['number', 'number',
                           'number', 'number', 'number',
                           'number', 'number', 'number', 'number',
                           'number', 'number', 'number'],
                          [this.ptr, timePointUs,
                           translation.x, translation.y, translation.z,
                           rotation.w, rotation.x, rotation.y, rotation.z,
                           scale.x, scale.y, scale.z]);
  }

  flush() {
    this.wasmModule.ccall('rgbd_file_writer_flush', null, ['number'], [this.ptr]);
  }

  getBytes() {
    const byteArrayPtr = this.wasmModule.ccall('rgbd_file_writer_get_bytes', 'number', ['number'], [this.ptr]);

    const byteArray = new NativeByteArray(this.wasmModule, byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}
