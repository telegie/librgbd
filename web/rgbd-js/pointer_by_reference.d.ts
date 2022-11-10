// Represents a void** in C.
export declare class PointerByReference {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any);
  close(): void;
  getValue(): number;
}
