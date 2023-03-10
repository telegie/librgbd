import { NativeQuaternion } from "./capi_containers";

export class Quaternion {
  w: number;
  x: number;
  y: number;
  z: number;

  constructor(w: number, x: number, y: number, z: number) {
    this.w = w;
    this.x = x;
    this.y = y;
    this.z = z;
  }

  static fromNative(nativeQuaternion: NativeQuaternion) {
    const w = nativeQuaternion.getW();
    const x = nativeQuaternion.getX();
    const y = nativeQuaternion.getY();
    const z = nativeQuaternion.getZ();
    return new Quaternion(w, x, y, z);
  }

  static createIdentity() {
    return new Quaternion(1, 0, 0, 0);
  }

  toNative(wasmModule: any) {
    const ptr = wasmModule.ccall('rgbd_native_quaternion_ctor',
                                 'number',
                                 ['number', 'number', 'number', 'number'],
                                 [this.w, this.x, this.y, this.z]);
    return new NativeQuaternion(wasmModule, ptr, true);
  }
}
