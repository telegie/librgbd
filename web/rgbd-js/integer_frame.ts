import { NativeInt32Array } from './capi_containers';

export class Int32Frame {
  width: number;
  height: number;
  values: Int32Array;

  constructor(nativeInt32Frame: NativeInt32Frame) {
    this.width = nativeInt32Frame.getWidth();
    this.height = nativeInt32Frame.getHeight();
    this.values = nativeInt32Frame.getValues();
  }
}

export class NativeInt32Frame {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_int32_frame_dtor', null, ['number'], [this.ptr]);
  }

  getWidth(): number {
    return this.wasmModule.ccall('rgbd_int32_frame_get_width', 'number', ['number'], [this.ptr]);
  }

  getHeight(): number {
    return this.wasmModule.ccall('rgbd_int32_frame_get_height', 'number', ['number'], [this.ptr]);
  }

  getValues(): Int32Array {
    const nativeValuesPtr = this.wasmModule.ccall('rgbd_int32_frame_get_values', 'number', ['number'], [this.ptr]);
    const nativeValues = new NativeInt32Array(this.wasmModule, nativeValuesPtr);
    const values = nativeValues.toArray();
    nativeValues.close();
    return values;
  }
}