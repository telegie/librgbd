export class NativeFileWriter {
  constructor(wasmModule, data) {
    this.wasmModule = wasmModule;
    this.ptr = this.wasmModule.ccall("rgbd_depth_decoder_ctor", "number", ["number"], [depthCodecType]);
  }
}