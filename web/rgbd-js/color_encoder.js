import { NativeByteArray } from './capi_containers.js';

export class NativeColorEncoder {
  constructor(wasmModule, colorCodecType, width, height, targetBitrate, framerate) {
    this.wasmModule = wasmModule;
    this.ptr = this.wasmModule.ccall('rgbd_color_encoder_ctor',
                                     'number',
                                     ['number', 'number', 'number', 'number', 'number'],
                                     [colorCodecType, width, height, targetBitrate, framerate]);
  }

  close() {
    this.wasmModule.ccall('rgbd_color_encoder_dtor', null, ['number'], [this.ptr]);
  }

  encode(yChannel, uChannel, vChannel, keyframe) {
    const yChannelPtr = this.wasmModule._malloc(yChannel.byteLength);
    const uChannelPtr = this.wasmModule._malloc(uChannel.byteLength);
    const vChannelPtr = this.wasmModule._malloc(vChannel.byteLength);
    this.wasmModule.HEAPU8.set(yChannel, yChannelPtr);
    this.wasmModule.HEAPU8.set(uChannel, uChannelPtr);
    this.wasmModule.HEAPU8.set(vChannel, vChannelPtr);
    const byteArrayPtr = this.wasmModule.ccall('rgbd_color_encoder_encode',
                                               'number',
                                               ['number', 'number', 'number', 'number', 'boolean'],
                                               [this.ptr, yChannelPtr, uChannelPtr, vChannelPtr, keyframe]);
    this.wasmModule._free(yChannelPtr);
    this.wasmModule._free(uChannelPtr);
    this.wasmModule._free(vChannelPtr);

    const byteArray = new NativeByteArray(this.wasmModule, byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}
