import { NativeAVPacketHandle } from "./av_packet_handle.js";


export class NativeColorEncoderFrame {
  constructor(wasmModule, ptr) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
  }

  close() {
    this.wasmModule.ccall('rgbd_color_encoder_frame_dtor', null, ['number'], [this.ptr]);
  }

  getPacket() {
    const packetPtr = this.wasmModule.ccall('rgbd_color_encoder_frame_get_packet', null, ['number'], [this.ptr]);
    return new NativeAVPacketHandle(this.wasmModule, packetPtr, false);
  }
}

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
    const colorEncoderFramePtr = this.wasmModule.ccall('rgbd_color_encoder_encode',
                                                       'number',
                                                       ['number', 'number', 'number', 'number', 'boolean'],
                                                       [this.ptr, yChannelPtr, uChannelPtr, vChannelPtr, keyframe]);
    this.wasmModule._free(yChannelPtr);
    this.wasmModule._free(uChannelPtr);
    this.wasmModule._free(vChannelPtr);

    const colorEncoderFrame = new NativeColorEncoderFrame(this.wasmModule, colorEncoderFramePtr);
    return colorEncoderFrame;
  }
}
