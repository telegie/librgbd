import { NativeByteArray } from "./capi_containers.js";

export class NativeDepthEncoder {
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  static createRVLEncoder(wasmModule, width, height) {
    const ptr = wasmModule.ccall("rgbd_depth_encoder_create_rvl_encoder",
                                 "number",
                                 ["number", "number"],
                                 [width, height]);
    return new NativeDepthEncoder(wasmModule, ptr);
  }

  static createTDC1Encoder(wasmModule, width, height, depthDiffMultiplier) {
    const ptr = wasmModule.ccall("rgbd_depth_encoder_create_tdc1_encoder",
                                 "number",
                                 ["number", "number", "number"],
                                 [width, height, depthDiffMultiplier]);
    return new NativeDepthEncoder(wasmModule, ptr);
  }

  close() {
    this.wasmModule.ccall("rgbd_depth_encoder_dtor", null, ["number"], [this.ptr]);
  }

  encode(depthValues, keyframe) {
    const depthValuesPtr = this.wasmModule._malloc(depthValues.byteLength);
    this.wasmModule.HEAPU8.set(depthValues, depthValuesPtr);
    const byteArrayPtr = this.wasmModule.ccall("rgbd_depth_encoder_encode",
                                               "number",
                                               ["number", "number", "boolean"],
                                               [this.ptr, depthValuesPtr, keyframe]);
    this.wasmModule._free(depthValuesPtr);

    const byteArray = new NativeByteArray(this.wasmModule, byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}