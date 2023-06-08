import { EmscriptenModuleEx } from "./emscripten_module_ex";

export class NativeObject {
  private module: EmscriptenModuleEx;
  private ptr: number;
  private owner: boolean;
  private closed: boolean;

  constructor(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    this.module = module;
    this.ptr = ptr;
    this.owner = owner;
    this.closed = false;
  }

  delete() {
    console.error("NativeObject.delete() not implemented");
  }

  close() {
    if (this.owner && !closed) {
      this.delete();
      this.closed = true;
    }
  }

  getModule(): EmscriptenModuleEx {
    return this.module;
  }

  getPtr(): number {
    return this.ptr;
  }
}
