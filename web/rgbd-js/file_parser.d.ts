export declare class NativeFileParser {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, data: Uint8Array);
  close(): void;
  parseNoFrames(): NativeFile;
  parseAllFrames(): NativeFile;
}
