export class NativeByteArray {
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall("rgbd_native_byte_array_dtor", null, ["number"], [this.ptr]);
  }

  toArray() {
    const dataPtr = this.wasmModule.ccall("rgbd_native_byte_array_get_data", "number", ["number"], [this.ptr]);
    const size = this.wasmModule.ccall("rgbd_native_byte_array_get_size", "number", ["number"], [this.ptr]);
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
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall("rgbd_native_float_array_dtor", null, ["number"], [this.ptr]);
  }

  toArray() {
    const dataPtr = this.wasmModule.ccall("rgbd_native_float_array_get_data", "number", ["number"], [this.ptr]);
    const size = this.wasmModule.ccall("rgbd_native_float_array_get_size", "number", ["number"], [this.ptr]);
    const array = new Float32Array(this.wasmModule.HEAPF32.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeInt32Array {
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall("rgbd_native_int32_array_dtor", null, ["number"], [this.ptr]);
  }

  toArray() {
    const dataPtr = this.wasmModule.ccall("rgbd_native_int32_array_get_data", "number", ["number"], [this.ptr]);
    const size = this.wasmModule.ccall("rgbd_native_int32_array_get_size", "number", ["number"], [this.ptr]);
    const array = new Int32Array(this.wasmModule.HEAP32.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeUInt8Array {
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall("rgbd_native_uint8_array_dtor", null, ["number"], [this.ptr]);
  }

  toArray() {
    const dataPtr = this.wasmModule.ccall("rgbd_native_uint8_array_get_data", "number", ["number"], [this.ptr]);
    const size = this.wasmModule.ccall("rgbd_native_uint8_array_get_size", "number", ["number"], [this.ptr]);
    const array = new Uint8Array(this.wasmModule.HEAPU8.buffer, dataPtr, size);

    return array.slice(0);
  }
}

export class NativeString {
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr
  }

  close() {
    this.wasmModule.ccall("rgbd_native_string_dtor", null, ["number"], [this.ptr]);
  }

  toString() {
    const cStrPtr = this.wasmModule.ccall("rgbd_native_string_get_c_str", "number", ["number"], [this.ptr]);
    return this.wasmModule.UTF8ToString(cStrPtr);
  }
}
