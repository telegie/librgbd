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
