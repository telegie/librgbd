export declare class YuvFrame {
  yChannel: Uint8Array;
  uChannel: Uint8Array;
  vChannel: Uint8Array;
  width: number;
  height: number;

  constructor(nativeYuvFrame: NativeYuvFrame);
}

export declare class NativeYuvFrame {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number);
  close(): void;
  getYChannel(): Uint8Array;
  getUChannel(): Uint8Array;
  getVChannel(): Uint8Array;
  getWidth(): number;
  getHeight(): number;
}