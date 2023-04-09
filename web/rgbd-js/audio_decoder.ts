import { NativeObject } from './native_object';
import { NativeFloatArray } from './capi_containers';

export class NativeAudioDecoder extends NativeObject {
  constructor(wasmModule: any) {
    const ptr = wasmModule.ccall('rgbd_audio_decoder_ctor', 'number', [], []);
    super(wasmModule, ptr, true);
  }

  delete() {
    this.wasmModule.ccall('rgbd_audio_decoder_dtor', null, ['number'], [this.ptr]);
  }

  decode(bytes: Uint8Array): Float32Array {
    const bytesPtr = this.wasmModule._malloc(bytes.byteLength);
    this.wasmModule.HEAPU8.set(bytes, bytesPtr);
    const nativeFloatArrayPtr = this.wasmModule.ccall('rgbd_audio_decoder_decode',
                                                      'number',
                                                      ['number', 'number', 'number'],
                                                      [this.ptr, bytesPtr, bytes.byteLength]);
    this.wasmModule._free(bytesPtr);

    const nativeFloatArray = new NativeFloatArray(this.wasmModule, nativeFloatArrayPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();
    return floatArray;
  }
}
