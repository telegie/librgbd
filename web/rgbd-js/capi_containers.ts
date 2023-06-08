import { Quaternion, Vector3 } from '@math.gl/core';
import { NativeObject } from './native_object';
import { EmscriptenModuleEx } from './emscripten_module_ex';

export class NativeByteArray extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number) {
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_native_byte_array_dtor', null, ['number'], [this.getPtr()]);
  }

  toArray(): Uint8Array {
    const dataPtr = this.getModule().ccall('rgbd_native_byte_array_get_data', 'number', ['number'], [this.getPtr()]);
    const size = this.getModule().ccall('rgbd_native_byte_array_get_size', 'number', ['number'], [this.getPtr()]);
    const array = new Uint8Array(this.getModule().HEAPU8.buffer, dataPtr, size);

    // slice(0) returns a copy of the array.
    // This is needed because array does constructed using the buffer and native pointer becomes
    // invalid after the native pointer becomes invalid,
    // and the point of having this toArray function is to create in object that will outlive
    // the native pointer.
    return array.slice(0);
  }
}

export class NativeFloatArray extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number) {
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_native_float_array_dtor', null, ['number'], [this.getPtr()]);
  }

  toArray(): Float32Array {
    const dataPtr = this.getModule().ccall('rgbd_native_float_array_get_data', 'number', ['number'], [this.getPtr()]);
    const size = this.getModule().ccall('rgbd_native_float_array_get_size', 'number', ['number'], [this.getPtr()]);
    const array = new Float32Array(this.getModule().HEAPF32.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeInt32Array extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number) {
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_native_int32_array_dtor', null, ['number'], [this.getPtr()]);
  }

  toArray(): Int32Array {
    const dataPtr = this.getModule().ccall('rgbd_native_int32_array_get_data', 'number', ['number'], [this.getPtr()]);
    const size = this.getModule().ccall('rgbd_native_int32_array_get_size', 'number', ['number'], [this.getPtr()]);
    const array = new Int32Array(this.getModule().HEAP32.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeQuaternion extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    super(module, ptr, owner);
  }

  delete() {
    this.getModule().ccall('rgbd_native_quaternion_dtor', null, ['number'], [this.getPtr()]);
  }

  static fromMathGL(module: EmscriptenModuleEx, quaternion: Quaternion) {
    const ptr = module.ccall('rgbd_native_quaternion_ctor',
                             'number',
                             ['number', 'number', 'number', 'number'],
                             [quaternion.w, quaternion.x, quaternion.y, quaternion.z]);
    return new NativeQuaternion(module, ptr, true);
  }

  toMathGL(): Quaternion {
    const w = this.getW();
    const x = this.getX();
    const y = this.getY();
    const z = this.getZ();
    return new Quaternion(x, y, z, w);
  }


  getW(): number {
    return this.getModule().ccall('rgbd_native_quaternion_get_w', 'number', ['number'], [this.getPtr()]);
  }

  getX(): number {
    return this.getModule().ccall('rgbd_native_quaternion_get_x', 'number', ['number'], [this.getPtr()]);
  }

  getY(): number {
    return this.getModule().ccall('rgbd_native_quaternion_get_y', 'number', ['number'], [this.getPtr()]);
  }

  getZ(): number {
    return this.getModule().ccall('rgbd_native_quaternion_get_z', 'number', ['number'], [this.getPtr()]);
  }
}

export class NativeUInt8Array extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number) {
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_native_uint8_array_dtor', null, ['number'], [this.getPtr()]);
  }

  toArray(): Uint8Array {
    const dataPtr = this.getModule().ccall('rgbd_native_uint8_array_get_data', 'number', ['number'], [this.getPtr()]);
    const size = this.getModule().ccall('rgbd_native_uint8_array_get_size', 'number', ['number'], [this.getPtr()]);
    const array = new Uint8Array(this.getModule().HEAPU8.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeString extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number) {
    super(module, ptr, true);
  }

  close() {
    this.getModule().ccall('rgbd_native_string_dtor', null, ['number'], [this.getPtr()]);
  }

  toString(): string {
    const cStrPtr = this.getModule().ccall('rgbd_native_string_get_c_str', 'number', ['number'], [this.getPtr()]);
    return this.getModule().UTF8ToString(cStrPtr);
  }
}

export class NativeVector3 extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    super(module, ptr, owner);
  }

  delete() {
    this.getModule().ccall('rgbd_native_vector3_dtor', null, ['number'], [this.getPtr()]);
  }

  static fromMathGL(module: EmscriptenModuleEx, vector3: Vector3) {
    const ptr = module.ccall('rgbd_native_vector3_ctor',
                             'number',
                             ['number', 'number', 'number'],
                             [vector3.x, vector3.y, vector3.z]);
    return new NativeVector3(module, ptr, true);
  }

  toMathGL(): Vector3 {
    const x = this.getX();
    const y = this.getY();
    const z = this.getZ();
    return new Vector3(x, y, z);
  }

  getX(): number {
    return this.getModule().ccall('rgbd_native_vector3_get_x', 'number', ['number'], [this.getPtr()]);
  }

  getY(): number {
    return this.getModule().ccall('rgbd_native_vector3_get_y', 'number', ['number'], [this.getPtr()]);
  }

  getZ(): number {
    return this.getModule().ccall('rgbd_native_vector3_get_z', 'number', ['number'], [this.getPtr()]);
  }
}
