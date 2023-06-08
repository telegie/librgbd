import { NativeByteArray, NativeUInt8Array } from './capi_containers';
import { EmscriptenModuleEx } from './emscripten_module_ex';
import { NativeObject } from './native_object';

export class YuvFrame {
  width: number;
  height: number;
  yChannel: Uint8Array;
  uChannel: Uint8Array;
  vChannel: Uint8Array;

  constructor(width: number,
              height: number,
              yChannel: Uint8Array,
              uChannel: Uint8Array,
              vChannel: Uint8Array) {
    this.width = width;
    this.height = height;
    this.yChannel = yChannel;
    this.uChannel = uChannel;
    this.vChannel = vChannel;
  }

  static fromNative(nativeYuvFrame: NativeYuvFrame) {
    const width = nativeYuvFrame.getWidth();
    const height = nativeYuvFrame.getHeight();
    const yChannel = nativeYuvFrame.getYChannel();
    const uChannel = nativeYuvFrame.getUChannel();
    const vChannel = nativeYuvFrame.getVChannel();
    return new YuvFrame(width, height, yChannel, uChannel, vChannel);
  }

  toNative(module: EmscriptenModuleEx) {
    const yChannelPtr = module._malloc(this.yChannel.byteLength);
    const uChannelPtr = module._malloc(this.uChannel.byteLength);
    const vChannelPtr = module._malloc(this.vChannel.byteLength);
    module.HEAPU8.set(this.yChannel, yChannelPtr);
    module.HEAPU8.set(this.uChannel, uChannelPtr);
    module.HEAPU8.set(this.vChannel, vChannelPtr);
    const ptr = module.ccall('rgbd_yuv_frame_ctor',
                             'number',
                             ['number', 'number', 'number', 'number', 'number'],
                             [this.width, this.height, yChannelPtr, uChannelPtr, vChannelPtr]);
    module._free(yChannelPtr);
    module._free(uChannelPtr);
    module._free(vChannelPtr);

    return new NativeYuvFrame(module, ptr);
  }

  getMkvCoverSized(module: EmscriptenModuleEx): YuvFrame {
    const nativeYuvFrame = this.toNative(module);
    const nativeCoverSized = nativeYuvFrame.getMkvCoverSized();
    const coverSized = YuvFrame.fromNative(nativeCoverSized);
    nativeYuvFrame.close();
    nativeCoverSized.close();
    return coverSized;
  }

  getPNGBytes(module: EmscriptenModuleEx): Uint8Array {
    const nativeYuvFrame = this.toNative(module);
    const pngBytes = nativeYuvFrame.getPNGBytes();
    nativeYuvFrame.close();
    return pngBytes;
  }
}

export class NativeYuvFrame extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number) {
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_yuv_frame_dtor', null, ['number'], [this.getPtr()]);
  }

  getMkvCoverSized(): NativeYuvFrame {
    const coverSizedPtr = this.getModule().ccall('rgbd_yuv_frame_get_mkv_cover_sized', 'number', ['number'], [this.getPtr()]);
    return new NativeYuvFrame(this.getModule(), coverSizedPtr);
  }

  getPNGBytes(): Uint8Array {
    const byteArrayPtr = this.getModule().ccall('rgbd_yuv_frame_get_png_bytes',
                                           'number',
                                           ['number'],
                                           [this.getPtr()]);

    const byteArray = new NativeByteArray(this.getModule(), byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }

  getYChannel(): Uint8Array {
    const nativeYChannelPtr = this.getModule().ccall('rgbd_yuv_frame_get_y_channel', 'number', ['number'], [this.getPtr()]);
    const nativeYChannel = new NativeUInt8Array(this.getModule(), nativeYChannelPtr);
    const yChannel = nativeYChannel.toArray();
    nativeYChannel.close();

    return yChannel;
  }

  getUChannel(): Uint8Array {
    const nativeUChannelPtr = this.getModule().ccall('rgbd_yuv_frame_get_u_channel', 'number', ['number'], [this.getPtr()]);
    const nativeUChannel = new NativeUInt8Array(this.getModule(), nativeUChannelPtr);
    const uChannel = nativeUChannel.toArray();
    nativeUChannel.close();

    return uChannel;
  }

  getVChannel(): Uint8Array {
    const nativeVChannelPtr = this.getModule().ccall('rgbd_yuv_frame_get_v_channel', 'number', ['number'], [this.getPtr()]);
    const nativeVChannel = new NativeUInt8Array(this.getModule(), nativeVChannelPtr);
    const vChannel = nativeVChannel.toArray();
    nativeVChannel.close();

    return vChannel;
  }

  getWidth(): number {
    return this.getModule().ccall('rgbd_yuv_frame_get_width', 'number', ['number'], [this.getPtr()]);
  }

  getHeight(): number {
    return this.getModule().ccall('rgbd_yuv_frame_get_height', 'number', ['number'], [this.getPtr()]);
  }
}