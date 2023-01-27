import { NativeByteArray, NativeUInt8Array } from './capi_containers';

export class YuvFrame {
  yChannel: Uint8Array;
  uChannel: Uint8Array;
  vChannel: Uint8Array;
  width: number;
  height: number;

  constructor(yChannel: Uint8Array,
              uChannel: Uint8Array,
              vChannel: Uint8Array,
              width: number,
              height: number) {
    this.yChannel = yChannel;
    this.uChannel = uChannel;
    this.vChannel = vChannel;
    this.width = width;
    this.height = height;
  }

  static fromNative(nativeYuvFrame: NativeYuvFrame) {
    const yChannel = nativeYuvFrame.getYChannel();
    const uChannel = nativeYuvFrame.getUChannel();
    const vChannel = nativeYuvFrame.getVChannel();
    const width = nativeYuvFrame.getWidth();
    const height = nativeYuvFrame.getHeight();
    return new YuvFrame(yChannel, uChannel, vChannel, width, height);
  }

  toNative(wasmModule: any) {
    const yChannelPtr = wasmModule._malloc(this.yChannel.byteLength);
    const uChannelPtr = wasmModule._malloc(this.uChannel.byteLength);
    const vChannelPtr = wasmModule._malloc(this.vChannel.byteLength);
    wasmModule.HEAPU8.set(this.yChannel, yChannelPtr);
    wasmModule.HEAPU8.set(this.uChannel, uChannelPtr);
    wasmModule.HEAPU8.set(this.vChannel, vChannelPtr);
    const ptr = wasmModule.ccall('rgbd_yuv_frame_ctor',
                                 'number',
                                 ['number', 'number', 'number', 'number', 'number'],
                                 [this.width, this.height, yChannelPtr, uChannelPtr, vChannelPtr]);
    wasmModule._free(yChannelPtr);
    wasmModule._free(uChannelPtr);
    wasmModule._free(vChannelPtr);

    return new NativeYuvFrame(wasmModule, ptr);
  }

  getMkvCoverSized(wasmModule: any): YuvFrame {
    const nativeYuvFrame = this.toNative(wasmModule);
    const nativeCoverSized = nativeYuvFrame.getMkvCoverSized();
    const coverSized = YuvFrame.fromNative(nativeCoverSized);
    nativeYuvFrame.close();
    nativeCoverSized.close();
    return coverSized;
  }

  getPNGBytes(wasmModule: any): Uint8Array {
    const nativeYuvFrame = this.toNative(wasmModule);
    const pngBytes = nativeYuvFrame.getPNGBytes();
    nativeYuvFrame.close();
    return pngBytes;
  }
}

export class NativeYuvFrame {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, ptr: number) {
    this.wasmModule = wasmModule;
    this.ptr = ptr
  }

  close() {
    this.wasmModule.ccall('rgbd_yuv_frame_dtor', null, ['number'], [this.ptr]);
  }

  getMkvCoverSized(): NativeYuvFrame {
    const coverSizedPtr = this.wasmModule.ccall('rgbd_yuv_frame_get_mkv_cover_sized', 'number', ['number'], [this.ptr]);
    return new NativeYuvFrame(this.wasmModule, coverSizedPtr);
  }

  getPNGBytes(): Uint8Array {
    const byteArrayPtr = this.wasmModule.ccall('rgbd_yuv_frame_get_png_bytes',
                                               'number',
                                               ['number'],
                                               [this.ptr]);

    const byteArray = new NativeByteArray(this.wasmModule, byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }

  getYChannel(): Uint8Array {
    const nativeYChannelPtr = this.wasmModule.ccall('rgbd_yuv_frame_get_y_channel', 'number', ['number'], [this.ptr]);
    const nativeYChannel = new NativeUInt8Array(this.wasmModule, nativeYChannelPtr);
    const yChannel = nativeYChannel.toArray();
    nativeYChannel.close();

    return yChannel;
  }

  getUChannel(): Uint8Array {
    const nativeUChannelPtr = this.wasmModule.ccall('rgbd_yuv_frame_get_u_channel', 'number', ['number'], [this.ptr]);
    const nativeUChannel = new NativeUInt8Array(this.wasmModule, nativeUChannelPtr);
    const uChannel = nativeUChannel.toArray();
    nativeUChannel.close();

    return uChannel;
  }

  getVChannel(): Uint8Array {
    const nativeVChannelPtr = this.wasmModule.ccall('rgbd_yuv_frame_get_v_channel', 'number', ['number'], [this.ptr]);
    const nativeVChannel = new NativeUInt8Array(this.wasmModule, nativeVChannelPtr);
    const vChannel = nativeVChannel.toArray();
    nativeVChannel.close();

    return vChannel;
  }

  getWidth(): number {
    return this.wasmModule.ccall('rgbd_yuv_frame_get_width', 'number', ['number'], [this.ptr]);
  }

  getHeight(): number {
    return this.wasmModule.ccall('rgbd_yuv_frame_get_height', 'number', ['number'], [this.ptr]);
  }
}