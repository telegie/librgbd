export class NativeFileWriterConfig {
  constructor(wasmModule) {
    this.wasmModule = wasmModule;
    this.ptr = this.wasmModule.ccall("rgbd_file_writer_config_ctor", "number", [], []);
  }

  close() {
    this.wasmModule.ccall("rgbd_file_writer_config_dtor", null, ["number"], [this.ptr]);
  }

  setFramerate(framerate) {
    this.wasmModule.ccall("rgbd_file_writer_config_set_framerate",
                          null,
                          ["number", "number"],
                          [this.ptr, framerate]);
  }

  setSamplerate(samplerate) {
    this.wasmModule.ccall("rgbd_file_writer_config_set_samplerate",
                          null,
                          ["number", "number"],
                          [this.ptr, samplerate]);
  }

  setDepthCodecType(depthCodecType) {
    this.wasmModule.ccall("rgbd_file_writer_config_set_depth_codec_type",
                          null,
                          ["number", "number"],
                          [this.ptr, depthCodecType]);
  }

  setDepthUnit(depthUnit) {
    this.wasmModule.ccall("rgbd_file_writer_config_set_depth_unit",
                          null,
                          ["number", "number"],
                          [this.ptr, depthUnit]);
  }
}

export class NativeFileWriter {
  constructor(wasmModule, calibration, nativeWriterConfig) {
    this.wasmModule = wasmModule;

    const nativeCalibration = calibration.createNativeInstance();
    this.ptr = this.wasmModule.ccall("rgbd_file_writer_ctor_in_memory",
                                     "number",
                                     ["number", "number"],
                                     [nativeCalibration.ptr, nativeWriterConfig.ptr]);
    nativeCalibration.close();
  }

  close() {
    this.wasmModule.ccall("rgbd_file_writer_dtor", null, ["number"], [this.ptr]);
  }
}