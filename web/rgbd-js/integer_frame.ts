import { NativeInt32Array } from './capi_containers';
import { EmscriptenModuleEx } from './emscripten_module_ex';
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

  toNative(module: EmscriptenModuleEx): NativeInt32Frame {
    const valuesPtr = module._malloc(this.values.byteLength);
    module.HEAP32.set(this.values, valuesPtr >> 2);
    const ptr = module.ccall('rgbd_int32_frame_ctor',
                             'number',
                             ['number', 'number', 'number'],
                             [this.width, this.height, valuesPtr]);
    module._free(valuesPtr);

    return new NativeInt32Frame(module, ptr);
  }
}

export class NativeInt32Frame extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number) {
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_int32_frame_dtor', null, ['number'], [this.getPtr()]);
  }

  getWidth(): number {
    return this.getModule().ccall('rgbd_int32_frame_get_width', 'number', ['number'], [this.getPtr()]);
  }

  getHeight(): number {
    return this.getModule().ccall('rgbd_int32_frame_get_height', 'number', ['number'], [this.getPtr()]);
  }

  getValues(): Int32Array {
    const nativeValuesPtr = this.getModule().ccall('rgbd_int32_frame_get_values', 'number', ['number'], [this.getPtr()]);
    const nativeValues = new NativeInt32Array(this.getModule(), nativeValuesPtr);
    const values = nativeValues.toArray();
    nativeValues.close();
    return values;
  }
}