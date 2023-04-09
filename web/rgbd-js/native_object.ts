export class NativeObject {
  wasmModule: any;
  ptr: number;
  owner: boolean;
  closed: boolean;

  constructor(wasmModule: any, ptr: number, owner: boolean) {
    this.wasmModule = wasmModule;
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
}
