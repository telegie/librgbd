import { Quaternion, Vector3 } from "@math.gl/core";
import { NativeQuaternion, NativeVector3 } from "./capi_containers"

export class MathUtils {
  static applyRotationRateAndGravityToRotation(wasmModule: any,
                                               rotation: Quaternion,                               
                                               deltaTimeSec: number,
                                               rotationRate: Vector3,
                                               gravity: Vector3): Quaternion {
    const nativeRotation = NativeQuaternion.fromMathGL(wasmModule, rotation);
    const nativeRotationRate = NativeVector3.fromMathGL(wasmModule, rotationRate);
    const nativeGravity = NativeVector3.fromMathGL(wasmModule, gravity);
    const nativeNewRotationPtr = wasmModule.ccall('rgbd_math_utils_apply_rotation_rate_and_gravity_to_rotation',
                                                  'number',
                                                  ['number', 'number', 'number', 'number'],
                                                  [nativeRotation.ptr, deltaTimeSec, nativeRotationRate.ptr, nativeGravity.ptr]);
    nativeRotation.close();
    nativeRotationRate.close();
    nativeGravity.close();

    const nativeNewRotation = new NativeQuaternion(wasmModule, nativeNewRotationPtr, true);
    const newRotation = nativeNewRotation.toMathGL();
    nativeNewRotation.close();
    return newRotation;
  }
}
