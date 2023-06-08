import { NativeByteArray } from './capi_containers';
import { EmscriptenModuleEx } from './emscripten_module_ex';
import { NativeObject } from './native_object';
import { YuvFrame } from './yuv_frame';

export class NativeColorEncoder extends NativeObject {
  constructor(module: EmscriptenModuleEx,
              colorCodecType: number,
              width: number,
              height: number) {
    const ptr = module.ccall('rgbd_color_encoder_ctor',
                             'number',
                             ['number', 'number', 'number'],
                             [colorCodecType, width, height]);
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_color_encoder_dtor', null, ['number'], [this.getPtr()]);
  }

  encode(yuvFrame: YuvFrame, keyframe: boolean): Uint8Array {
    const nativeYuvFrame = yuvFrame.toNative(this.getModule());
    const byteArrayPtr = this.getModule().ccall('rgbd_color_encoder_encode',
                                           'number',
                                           ['number', 'number', 'boolean'],
                                           [this.getPtr(), nativeYuvFrame.getPtr(), keyframe]);
    nativeYuvFrame.close();

    const byteArray = new NativeByteArray(this.getModule(), byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}
