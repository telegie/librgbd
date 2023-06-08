import { EmscriptenModuleEx } from './emscripten_module_ex';
import { Int32Frame, NativeInt32Frame } from './integer_frame';
import { NativeObject } from './native_object';

export enum DepthCodecType {
  RVL = 0,
  TDC1 = 1
}

export class NativeDepthDecoder extends NativeObject {
  constructor(module: EmscriptenModuleEx, depthCodecType: DepthCodecType) {
    const ptr = module.ccall('rgbd_depth_decoder_ctor', 'number', ['number'], [depthCodecType]);
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_depth_decoder_dtor', null, ['number'], [this.getPtr()]);
  }

  decode(depthBytes: Uint8Array): Int32Frame {
    const depthBytesPtr = this.getModule()._malloc(depthBytes.byteLength);
    this.getModule().HEAPU8.set(depthBytes, depthBytesPtr);
    const nativeInt32FramePtr = this.getModule().ccall('rgbd_depth_decoder_decode',
                                                  'number',
                                                  ['number', 'number', 'number'],
                                                  [this.getPtr(), depthBytesPtr, depthBytes.byteLength]);
    this.getModule()._free(depthBytesPtr);

    const nativeInt32Frame = new NativeInt32Frame(this.getModule(), nativeInt32FramePtr);
    const int32Frame = Int32Frame.fromNative(nativeInt32Frame);
    nativeInt32Frame.close();
    return int32Frame;
  }
}
