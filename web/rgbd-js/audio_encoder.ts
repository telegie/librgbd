import { NativeObject } from './native_object';
import { NativeByteArray } from './capi_containers';
import { EmscriptenModuleEx } from './emscripten_module_ex';

export function AUDIO_INPUT_SAMPLES_PER_FRAME(module: EmscriptenModuleEx) {
  return module.ccall('RGBD_AUDIO_INPUT_SAMPLES_PER_FRAME', null, [], []);
}

export function AUDIO_SAMPLE_RATE(module: EmscriptenModuleEx) {
  return module.ccall('RGBD_AUDIO_SAMPLE_RATE', null, [], []);
}

export class AudioEncoderFrame {
  packetBytesList: Uint8Array[];
  constructor(packetBytesList: Uint8Array[]) {
    this.packetBytesList = packetBytesList;
  }

  static fromNative(nativeFrame: NativeAudioEncoderFrame) {
    let packetBytesList: Uint8Array[] = [];
    const packetBytesListCount = nativeFrame.getPacketBytesListCount();
    for (let i = 0; i < packetBytesListCount; ++i) {
      const packetBytes = nativeFrame.getPacketBytes(i);
      packetBytesList.push(packetBytes);
    }
    return new AudioEncoderFrame(packetBytesList);
  }
}

class NativeAudioEncoderFrame extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number) {
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_audio_encoder_frame_dtor', null, ['number'], [this.getPtr()]);
  }

  getPacketBytesListCount(): number {
    return this.getModule().ccall('rgbd_audio_encoder_frame_get_packet_bytes_list_count',
                             'number',
                             ['number'],
                             [this.getPtr()]);
  }

  getPacketBytes(index: number): Uint8Array {
    const byteArrayPtr = this.getModule().ccall('rgbd_audio_encoder_frame_get_packet_bytes',
                                           'number',
                                           ['number', 'number'],
                                           [this.getPtr(), index]);
    const byteArray = new NativeByteArray(this.getModule(), byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}

export class NativeAudioEncoder extends NativeObject {
  constructor(module: EmscriptenModuleEx) {
    const ptr = module.ccall('rgbd_audio_encoder_ctor', 'number', [], []);
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_audio_encoder_dtor', null, ['number'], [this.getPtr()]);
  }

  encode(pcmSamples: Float32Array) {
    const pcmSamplesPtr = this.getModule()._malloc(pcmSamples.byteLength);
    // Have to do >> 2 to the pointer since the set() function interprets its second parameter
    // as an index, not a pointer.
    // https://github.com/emscripten-core/emscripten/issues/4003
    this.getModule().HEAPF32.set(pcmSamples, pcmSamplesPtr >> 2);
    const framePtr = this.getModule().ccall('rgbd_audio_encoder_encode',
                                       'number',
                                       ['number', 'number', 'number'],
                                       [this.getPtr(), pcmSamplesPtr, pcmSamples.length]);
    this.getModule()._free(pcmSamplesPtr);

    const nativeFrame = new NativeAudioEncoderFrame(this.getModule(), framePtr);
    const frame = AudioEncoderFrame.fromNative(nativeFrame);
    nativeFrame.close();
    return frame;
  }

  flush() {
    const framePtr = this.getModule().ccall('rgbd_audio_encoder_flush',
                                       'number',
                                       ['number'],
                                       [this.getPtr()]);

    const nativeFrame = new NativeAudioEncoderFrame(this.getModule(), framePtr);
    const frame = AudioEncoderFrame.fromNative(nativeFrame);
    nativeFrame.close();
    return frame;
  }
}
