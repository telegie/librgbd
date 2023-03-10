import { Quaternion } from "./quaternion"
import { Vector3 } from "./vector3"
import { NativeQuaternion } from "./capi_containers"

export class MathUtils {
  static applyRotationRateAndGravityToRotation(wasmModule: any,
                                               rotation: Quaternion,                               
                                               deltaTimeSec: number,
                                               rotationRate: Vector3,
                                               gravity: Vector3): Quaternion {
    const nativeRotation = rotation.toNative(wasmModule);
    const nativeRotationRate = rotationRate.toNative(wasmModule);
    const nativeGravity = gravity.toNative(wasmModule);
    const nativeNewRotationPtr = wasmModule.ccall('rgbd_math_utils_apply_rotation_rate_and_gravity_to_rotation',
                                                  'number',
                                                  ['number', 'number', 'number', 'number'],
                                                  [nativeRotation.ptr, deltaTimeSec, nativeRotationRate.ptr, nativeGravity.ptr]);
    nativeRotation.close();
    nativeRotationRate.close();
    nativeGravity.close();

    const nativeNewRotation = new NativeQuaternion(wasmModule, nativeNewRotationPtr, true);
    const newRotation = Quaternion.fromNative(nativeNewRotation);
    nativeNewRotation.close();
    return newRotation;
  }
}
