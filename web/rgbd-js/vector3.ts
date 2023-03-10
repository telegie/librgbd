import { NativeVector3 } from "./capi_containers";

export class Vector3 {
  x: number;
  y: number;
  z: number;

  constructor(x: number, y: number, z: number) {
    this.x = x;
    this.y = y;
    this.z = z;
  }

  static fromNative(nativeVector3: NativeVector3) {
    const x = nativeVector3.getX();
    const y = nativeVector3.getY();
    const z = nativeVector3.getZ();
    return new Vector3(x, y, z);
  }

  toNative(wasmModule: any) {
    const ptr = wasmModule.ccall('rgbd_native_vector3_ctor',
                                 'number',
                                 ['number', 'number', 'number'],
                                 [this.x, this.y, this.z]);
    return new NativeVector3(wasmModule, ptr, true);
  }

  multiply(factor: number): Vector3 {
    return new Vector3(this.x * factor, this.y * factor, this.z * factor);
  }

  dot(v: Vector3): number {
    return this.x * v.x + this.y * v.y + this.z * v.z;
  }
}
