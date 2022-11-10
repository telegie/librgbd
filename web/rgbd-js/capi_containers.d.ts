export declare class NativeByteArray {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number);
  close(): void;
  toArray(): Uint8Array;
}

export declare class NativeFloatArray {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number);
  close(): void;
  toArray(): Float32Array;
}

export class NativeInt32Array {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number);
  close(): void;
  toArray(): Int32Array;
}

export class NativeUInt8Array {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number);
  close(): void;
  toArray(): Uint8Array;
}

export class NativeString {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number);
  close(): void;
  toString(): string;
}
