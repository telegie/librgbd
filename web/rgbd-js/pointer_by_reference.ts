import { EmscriptenModuleEx } from "./emscripten_module_ex";
import { NativeObject } from "./native_object";

// Represents a void** in C.
export class PointerByReference extends NativeObject {
  constructor(module: EmscriptenModuleEx) {
    const ptr = module.ccall('rgbd_pointer_by_reference_ctor', 'number', [], []);
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_pointer_by_reference_dtor', null, ['number'], [this.getPtr()]);
  }

  getValue(): number {
    return this.getModule().ccall('rgbd_pointer_by_reference_get_value', 'number', ['number'], [this.getPtr()]);
  }
}
