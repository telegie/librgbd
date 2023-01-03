import { NativeUInt8Array } from './capi_containers';

export class YuvFrame {
  yChannel: Uint8Array;
  uChannel: Uint8Array;
  vChannel: Uint8Array;
  width: number;
  height: number;

  constructor(nativeYuvFrame: NativeYuvFrame) {
    this.yChannel = nativeYuvFrame.getYChannel();
    this.uChannel = nativeYuvFrame.getUChannel();
    this.vChannel = nativeYuvFrame.getVChannel();
    this.width = nativeYuvFrame.getWidth();
    this.height = nativeYuvFrame.getHeight();
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