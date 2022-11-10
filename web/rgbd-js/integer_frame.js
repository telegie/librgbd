import { NativeInt32Array } from "./capi_containers.js";

export class Int32Frame {
  constructor(nativeInt32Frame) {
    this.width = nativeInt32Frame.getWidth();
    this.height = nativeInt32Frame.getHeight();
    this.values = nativeInt32Frame.getValues();
  }
}

export class NativeInt32Frame {
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall("rgbd_int32_frame_dtor", null, ["number"], [this.ptr]);
  }

  getWidth() {
    return this.wasmModule.ccall("rgbd_int32_frame_get_width", "number", ["number"], [this.ptr]);
  }

  getHeight() {
    return this.wasmModule.ccall("rgbd_int32_frame_get_height", "number", ["number"], [this.ptr]);
  }

  getValues() {
    const nativeValuesPtr = this.wasmModule.ccall("rgbd_int32_frame_get_values", "number", ["number"], [this.ptr]);
    const nativeValues = new NativeInt32Array(this.wasmModule, nativeValuesPtr);
    const values = nativeValues.toArray();
    nativeValues.close();
    return values;
  }
}