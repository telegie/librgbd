export declare class NativeAudioDecoder {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any);
  close(): void;
  decode(bytes: Uint8Array): Float32Array;
}
