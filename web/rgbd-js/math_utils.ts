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

  static convertEulerAnglesToQuaternion(wasmModule: any, eulerAngles: Vector3): Quaternion {
    const nativeEulerAngles = NativeVector3.fromMathGL(wasmModule, eulerAngles);
    const nativeQuaternionPtr = wasmModule.ccall('rgbd_math_utils_convert_euler_angles_to_quaternion',
                                                 'number',
                                                 ['number'],
                                                 [nativeEulerAngles.ptr]);
    nativeEulerAngles.close();

    const nativeQuaternion = new NativeQuaternion(wasmModule, nativeQuaternionPtr, true);
    const quat = nativeQuaternion.toMathGL();
    nativeQuaternion.close();
    return quat;
  }

  static convertQuaternionToEulerAngles(wasmModule: any, quat: Quaternion): Vector3 {
    const nativeQuaternion = NativeQuaternion.fromMathGL(wasmModule, quat);
    const nativeEulerAnglesPtr = wasmModule.ccall('rgbd_math_utils_convert_quaternion_to_euler_angles',
                                                  'number',
                                                  ['number'],
                                                  [nativeQuaternion.ptr]);
    nativeQuaternion.close();

    const nativeEulerAngles = new NativeVector3(wasmModule, nativeEulerAnglesPtr, true);
    const eulerAngles = nativeEulerAngles.toMathGL();
    nativeEulerAngles.close();
    return eulerAngles;
  }
}
