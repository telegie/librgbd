import { Int32Frame, NativeInt32Frame } from './integer_frame';
import { NativeObject } from './native_object';

export enum DepthCodecType {
  RVL = 0,
  TDC1 = 1
}

export class NativeDepthDecoder extends NativeObject {
  constructor(wasmModule: any, depthCodecType: DepthCodecType) {
    const ptr = wasmModule.ccall('rgbd_depth_decoder_ctor', 'number', ['number'], [depthCodecType]);
    super(wasmModule, ptr, true);
  }

  delete() {
    this.wasmModule.ccall('rgbd_depth_decoder_dtor', null, ['number'], [this.ptr]);
  }

  decode(depthBytes: Uint8Array): Int32Frame {
    const depthBytesPtr = this.wasmModule._malloc(depthBytes.byteLength);
    this.wasmModule.HEAPU8.set(depthBytes, depthBytesPtr);
    const nativeInt32FramePtr = this.wasmModule.ccall('rgbd_depth_decoder_decode',
                                                      'number',
                                                      ['number', 'number', 'number'],
                                                      [this.ptr, depthBytesPtr, depthBytes.byteLength]);
    this.wasmModule._free(depthBytesPtr);

    const nativeInt32Frame = new NativeInt32Frame(this.wasmModule, nativeInt32FramePtr);
    const int32Frame = Int32Frame.fromNative(nativeInt32Frame);
    nativeInt32Frame.close();
    return int32Frame;
  }
}
