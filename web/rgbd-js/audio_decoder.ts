import { NativeObject } from './native_object';
import { NativeFloatArray } from './capi_containers';
import { EmscriptenModuleEx } from './emscripten_module_ex';

export class NativeAudioDecoder extends NativeObject {
  constructor(module: EmscriptenModuleEx) {
    const ptr = module.ccall('rgbd_audio_decoder_ctor', 'number', [], []);
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_audio_decoder_dtor', null, ['number'], [this.getPtr()]);
  }

  decode(bytes: Uint8Array): Float32Array {
    const bytesPtr = this.getModule()._malloc(bytes.byteLength);
    this.getModule().HEAPU8.set(bytes, bytesPtr);
    const nativeFloatArrayPtr = this.getModule().ccall('rgbd_audio_decoder_decode',
                                                      'number',
                                                      ['number', 'number', 'number'],
                                                      [this.getPtr(), bytesPtr, bytes.byteLength]);
    this.getModule()._free(bytesPtr);

    const nativeFloatArray = new NativeFloatArray(this.getModule(), nativeFloatArrayPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();
    return floatArray;
  }
}
