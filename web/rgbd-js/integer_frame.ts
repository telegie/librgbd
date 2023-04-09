import { NativeInt32Array } from './capi_containers';
import { NativeObject } from './native_object';

export class Int32Frame {
  width: number;
  height: number;
  values: Int32Array;

  constructor(width: number,
              height: number,
              values: Int32Array) {
    this.width = width;
    this.height = height;
    this.values = values;
  }

  static fromNative(nativeInt32Frame: NativeInt32Frame): Int32Frame {
    const width = nativeInt32Frame.getWidth();
    const height = nativeInt32Frame.getHeight();
    const values = nativeInt32Frame.getValues();
    return new Int32Frame(width, height, values);
  }

  toNative(wasmModule: any): NativeInt32Frame {
    const valuesPtr = wasmModule._malloc(this.values.byteLength);
    wasmModule.HEAP32.set(this.values, valuesPtr >> 2);
    const ptr = wasmModule.ccall('rgbd_int32_frame_ctor',
                                 'number',
                                 ['number', 'number', 'number'],
                                 [this.width, this.height, valuesPtr]);
    wasmModule._free(valuesPtr);

    return new NativeInt32Frame(wasmModule, ptr);
  }
}

export class NativeInt32Frame extends NativeObject {
  constructor(wasmModule: any, ptr: number) {
    super(wasmModule, ptr, true);
  }

  delete() {
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