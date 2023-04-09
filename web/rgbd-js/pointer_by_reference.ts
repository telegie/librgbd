import { NativeObject } from "./native_object";

// Represents a void** in C.
export class PointerByReference extends NativeObject {
  constructor(wasmModule: any) {
    const ptr = wasmModule.ccall('rgbd_pointer_by_reference_ctor', 'number', [], []);
    super(wasmModule, ptr, true);
  }

  delete() {
    this.wasmModule.ccall('rgbd_pointer_by_reference_dtor', null, ['number'], [this.ptr]);
  }

  getValue(): number {
    return this.wasmModule.ccall('rgbd_pointer_by_reference_get_value', 'number', ['number'], [this.ptr]);
  }
}
