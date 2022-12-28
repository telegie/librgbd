export class AudioEncoderFrame {
  packetBytesList: Uint8Array[];
}

export declare class NativeAudioEncoder {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any);
  close(): void;
  encode(pcmSamples: number[]): AudioEncoderFrame;
  flush(): AudioEncoderFrame;
}
