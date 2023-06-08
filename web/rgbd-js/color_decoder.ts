import { EmscriptenModuleEx } from './emscripten_module_ex';
import { NativeObject } from './native_object';
import { NativeYuvFrame, YuvFrame } from './yuv_frame';

export enum ColorCodecType {
  VP8 = 0
}

export class NativeColorDecoder extends NativeObject {
  constructor(module: EmscriptenModuleEx, colorCodecType: ColorCodecType) {
    const ptr = module.ccall('rgbd_color_decoder_ctor', 'number', ['number'], [colorCodecType]);
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_color_decoder_dtor', null, ['number'], [this.getPtr()]);
  }

  decode(colorBytes: Uint8Array): YuvFrame {
    const colorBytesPtr = this.getModule()._malloc(colorBytes.byteLength);
    this.getModule().HEAPU8.set(colorBytes, colorBytesPtr);
    const yuvFramePtr = this.getModule().ccall('rgbd_color_decoder_decode',
                                          'number',
                                          ['number', 'number', 'number'],
                                          [this.getPtr(), colorBytesPtr, colorBytes.byteLength]);
    this.getModule()._free(colorBytesPtr);

    const nativeYuvFrame = new NativeYuvFrame(this.getModule(), yuvFramePtr);
    const yuvFrame = YuvFrame.fromNative(nativeYuvFrame);
    nativeYuvFrame.close();
    return yuvFrame;
  }
}
