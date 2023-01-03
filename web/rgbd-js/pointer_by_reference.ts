// Represents a void** in C.
export class PointerByReference {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any) {
    this.wasmModule = wasmModule;
    this.ptr = wasmModule.ccall('rgbd_pointer_by_reference_ctor', 'number', [], []);
  }

  close() {
    this.wasmModule.ccall('rgbd_pointer_by_reference_dtor', null, ['number'], [this.ptr]);
  }

  getValue(): number {
    return this.wasmModule.ccall('rgbd_pointer_by_reference_get_value', 'number', ['number'], [this.ptr]);
  }
}
