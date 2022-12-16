import { NativeByteArray } from "./capi_containers.js";

export class NativeAVPacketHandle {
  constructor(wasmModule, ptr, owner) {
    this.wasmModule = wasmModule;
    this.ptr = ptr;
    this.owner = owner;
  }

  close() {
    if (this.owner)
      this.wasmModule.ccall("rgbd_av_packet_handle_dtor", null, ["number"], [this.ptr]);
  }

  getDataBytes() {
    const byteArrayPtr = this.wasmModule.ccall("rgbd_av_packet_handle_get_data_bytes", "number", ["number"], [this.ptr]);
    return new NativeByteArray(this.wasmModule, byteArrayPtr).toArray();
  }
}
