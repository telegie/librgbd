import { NativeByteArray } from './capi_containers';

export function AUDIO_INPUT_SAMPLES_PER_FRAME(wasmModule: any) {
  return wasmModule.ccall('RGBD_AUDIO_INPUT_SAMPLES_PER_FRAME', null, [], []);
}

export function AUDIO_SAMPLE_RATE(wasmModule: any) {
  return wasmModule.ccall('RGBD_AUDIO_SAMPLE_RATE', null, [], []);
}

export class AudioEncoderFrame {
  packetBytesList: Uint8Array[];
  constructor(nativeFrame: NativeAudioEncoderFrame) {
    let packetBytesList: Uint8Array[] = [];
    const packetBytesListCount = nativeFrame.getPacketBytesListCount();
    for (let i = 0; i < packetBytesListCount; ++i) {
      const packetBytes = nativeFrame.getPacketBytes(i);
      packetBytesList.push(packetBytes);
    }
    this.packetBytesList = packetBytesList;
  }
}

class NativeAudioEncoderFrame {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_audio_encoder_frame_dtor', null, ['number'], [this.ptr]);
  }

  getPacketBytesListCount(): number {
    return this.wasmModule.ccall('rgbd_audio_encoder_frame_get_packet_bytes_list_count',
                                 'number',
                                 ['number'],
                                 [this.ptr]);
  }

  getPacketBytes(index: number): Uint8Array {
    const byteArrayPtr = this.wasmModule.ccall('rgbd_audio_encoder_frame_get_packet_bytes',
                                               'number',
                                               ['number', 'number'],
                                               [this.ptr, index]);
    const byteArray = new NativeByteArray(this.wasmModule, byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}

export class NativeAudioEncoder {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any) {
    this.wasmModule = wasmModule;
    this.ptr = this.wasmModule.ccall('rgbd_audio_encoder_ctor', 'number', [], []);
  }

  close() {
    this.wasmModule.ccall('rgbd_audio_encoder_dtor', null, ['number'], [this.ptr]);
  }

  encode(pcmSamples: Float32Array) {
    const pcmSamplesPtr = this.wasmModule._malloc(pcmSamples.byteLength);
    // Have to do >> 2 to the pointer since the set() function interprets its second parameter
    // as an index, not a pointer.
    // https://github.com/emscripten-core/emscripten/issues/4003
    this.wasmModule.HEAPF32.set(pcmSamples, pcmSamplesPtr >> 2);
    const framePtr = this.wasmModule.ccall('rgbd_audio_encoder_encode',
                                           'number',
                                           ['number', 'number', 'number'],
                                           [this.ptr, pcmSamplesPtr, pcmSamples.length]);
    this.wasmModule._free(pcmSamplesPtr);

    const nativeFrame = new NativeAudioEncoderFrame(this.wasmModule, framePtr);
    const frame = new AudioEncoderFrame(nativeFrame);
    nativeFrame.close();
    return frame;
  }

  flush() {
    const framePtr = this.wasmModule.ccall('rgbd_audio_encoder_flush',
                                           'number',
                                           ['number'],
                                           [this.ptr]);

    const nativeFrame = new NativeAudioEncoderFrame(this.wasmModule, framePtr);
    const frame = new AudioEncoderFrame(nativeFrame);
    nativeFrame.close();
    return frame;
  }
}