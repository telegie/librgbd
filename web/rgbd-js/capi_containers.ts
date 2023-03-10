import { Quaternion, Vector3 } from '@math.gl/core';

export class NativeByteArray {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_native_byte_array_dtor', null, ['number'], [this.ptr]);
  }

  toArray(): Uint8Array {
    const dataPtr = this.wasmModule.ccall('rgbd_native_byte_array_get_data', 'number', ['number'], [this.ptr]);
    const size = this.wasmModule.ccall('rgbd_native_byte_array_get_size', 'number', ['number'], [this.ptr]);
    const array = new Uint8Array(this.wasmModule.HEAPU8.buffer, dataPtr, size);

    // slice(0) returns a copy of the array.
    // This is needed because array does constructed using the buffer and native pointer becomes
    // invalid after the native pointer becomes invalid,
    // and the point of having this toArray function is to create in object that will outlive
    // the native pointer.
    return array.slice(0);
  }
}

export class NativeFloatArray {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_native_float_array_dtor', null, ['number'], [this.ptr]);
  }

  toArray(): Float32Array {
    const dataPtr = this.wasmModule.ccall('rgbd_native_float_array_get_data', 'number', ['number'], [this.ptr]);
    const size = this.wasmModule.ccall('rgbd_native_float_array_get_size', 'number', ['number'], [this.ptr]);
    const array = new Float32Array(this.wasmModule.HEAPF32.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeInt32Array {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_native_int32_array_dtor', null, ['number'], [this.ptr]);
  }

  toArray(): Int32Array {
    const dataPtr = this.wasmModule.ccall('rgbd_native_int32_array_get_data', 'number', ['number'], [this.ptr]);
    const size = this.wasmModule.ccall('rgbd_native_int32_array_get_size', 'number', ['number'], [this.ptr]);
    const array = new Int32Array(this.wasmModule.HEAP32.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeQuaternion {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_native_quaternion_dtor', null, ['number'], [this.ptr]);
  }

  static fromMathGL(wasmModule: any, quaternion: Quaternion) {
    const ptr = wasmModule.ccall('rgbd_native_quaternion_ctor',
                                 'number',
                                 ['number', 'number', 'number', 'number'],
                                 [quaternion.w, quaternion.x, quaternion.y, quaternion.z]);
    return new NativeQuaternion(wasmModule, ptr, true);
  }

  toMathGL(): Quaternion {
    const w = this.getW();
    const x = this.getX();
    const y = this.getY();
    const z = this.getZ();
    return new Quaternion(x, y, z, w);
  }


  getW(): number {
    return this.wasmModule.ccall('rgbd_native_quaternion_get_w', 'number', ['number'], [this.ptr]);
  }

  getX(): number {
    return this.wasmModule.ccall('rgbd_native_quaternion_get_x', 'number', ['number'], [this.ptr]);
  }

  getY(): number {
    return this.wasmModule.ccall('rgbd_native_quaternion_get_y', 'number', ['number'], [this.ptr]);
  }

  getZ(): number {
    return this.wasmModule.ccall('rgbd_native_quaternion_get_z', 'number', ['number'], [this.ptr]);
  }
}

export class NativeUInt8Array {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_native_uint8_array_dtor', null, ['number'], [this.ptr]);
  }

  toArray(): Uint8Array {
    const dataPtr = this.wasmModule.ccall('rgbd_native_uint8_array_get_data', 'number', ['number'], [this.ptr]);
    const size = this.wasmModule.ccall('rgbd_native_uint8_array_get_size', 'number', ['number'], [this.ptr]);
    const array = new Uint8Array(this.wasmModule.HEAPU8.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeString {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr
  }

  close() {
    this.wasmModule.ccall('rgbd_native_string_dtor', null, ['number'], [this.ptr]);
  }

  toString(): string {
    const cStrPtr = this.wasmModule.ccall('rgbd_native_string_get_c_str', 'number', ['number'], [this.ptr]);
    return this.wasmModule.UTF8ToString(cStrPtr);
  }
}

export class NativeVector3 {
  wasmModule: any;
  ptr: number;
  owner: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall('rgbd_native_vector3_dtor', null, ['number'], [this.ptr]);
  }

  static fromMathGL(wasmModule: any, vector3: Vector3) {
    const ptr = wasmModule.ccall('rgbd_native_vector3_ctor',
                                 'number',
                                 ['number', 'number', 'number'],
                                 [vector3.x, vector3.y, vector3.z]);
    return new NativeVector3(wasmModule, ptr, true);
  }

  toMathGL(): Vector3 {
    const x = this.getX();
    const y = this.getY();
    const z = this.getZ();
    return new Vector3(x, y, z);
  }

  getX(): number {
    return this.wasmModule.ccall('rgbd_native_vector3_get_x', 'number', ['number'], [this.ptr]);
  }

  getY(): number {
    return this.wasmModule.ccall('rgbd_native_vector3_get_y', 'number', ['number'], [this.ptr]);
  }

  getZ(): number {
    return this.wasmModule.ccall('rgbd_native_vector3_get_z', 'number', ['number'], [this.ptr]);
  }
}
