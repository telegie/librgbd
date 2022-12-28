import { Int32Frame, NativeInt32Frame } from './integer_frame.js';

export const DEPTH_CODEC_TYPE_RVL = 0;
export const DEPTH_CODEC_TYPE_TDC1 = 1;

export class NativeDepthDecoder {
  constructor(wasmModule, depthCodecType) {
    this.wasmModule = wasmModule;
    this.ptr = this.wasmModule.ccall('rgbd_depth_decoder_ctor', 'number', ['number'], [depthCodecType]);
  }

  close() {
    this.wasmModule.ccall('rgbd_depth_decoder_dtor', null, ['number'], [this.ptr]);
  }

  decode(depthBytes) {
    const depthBytesPtr = this.wasmModule._malloc(depthBytes.byteLength);
    this.wasmModule.HEAPU8.set(depthBytes, depthBytesPtr);
    const nativeInt32FramePtr = this.wasmModule.ccall('rgbd_depth_decoder_decode',
                                                      'number',
                                                      ['number', 'number', 'number'],
                                                      [this.ptr, depthBytesPtr, depthBytes.byteLength]);
    this.wasmModule._free(depthBytesPtr);

    const nativeInt32Frame = new NativeInt32Frame(this.wasmModule, nativeInt32FramePtr);
    const int32Frame = new Int32Frame(nativeInt32Frame);
    nativeInt32Frame.close();
    return int32Frame;
  }
}
