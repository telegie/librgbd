import { NativeByteArray } from './capi_containers';
import { YuvFrame } from './yuv_frame';

export class NativeColorEncoder {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any,
              colorCodecType: number,
              width: number,
              height: number) {
    this.wasmModule = wasmModule;
    this.ptr = this.wasmModule.ccall('rgbd_color_encoder_ctor',
                                     'number',
                                     ['number', 'number', 'number'],
                                     [colorCodecType, width, height]);
  }

  close() {
    this.wasmModule.ccall('rgbd_color_encoder_dtor', null, ['number'], [this.ptr]);
  }

  encode(yuvFrame: YuvFrame, keyframe: boolean): Uint8Array {
    const nativeYuvFrame = yuvFrame.toNative(this.wasmModule);
    const byteArrayPtr = this.wasmModule.ccall('rgbd_color_encoder_encode',
                                               'number',
                                               ['number', 'number', 'boolean'],
                                               [this.ptr, nativeYuvFrame.ptr, keyframe]);
    nativeYuvFrame.close();

    const byteArray = new NativeByteArray(this.wasmModule, byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}
