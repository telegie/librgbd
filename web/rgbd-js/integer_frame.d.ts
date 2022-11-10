export declare class Int32Frame {
  width: number;
  height: number;
  values: Int32Array;

  constructor(nativeInt32Frame: NativeInt32Frame);
}

export declare class NativeInt32Frame {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number);
  close(): void;
  getWidth(): number;
  getHeight(): number;
  getValues(): Int32Array;
}
