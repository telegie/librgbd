import { Quaternion } from "./quaternion"
import { Vector3 } from "./vector3"
import { NativeQuaternion, NativeVector3 } from "./capi_containers"

export class MathUtils {
  static computeGravityCompensatingEulerAngles(wasmModule: any, gravity: Vector3): Vector3 {
    const nativeEulerAnglesPtr = wasmModule.ccall('rgbd_math_utils_compute_gravity_compensating_euler_angles',
                                                  'number',
                                                  ['number', 'number', 'number'],
                                                  [gravity.x, gravity.y, gravity.z]);

    const nativeEulerAngles = new NativeVector3(wasmModule, nativeEulerAnglesPtr, true);
    const eulerAngles = Vector3.fromNative(nativeEulerAngles);
    nativeEulerAngles.close();
    return eulerAngles;
  }

  static computeGravityCompensatingRotation(wasmModule: any, gravity: Vector3): Quaternion {
    const nativeRotationPtr = wasmModule.ccall('rgbd_math_utils_compute_gravity_compensating_rotation',
                                               'number',
                                               ['number', 'number', 'number'],
                                               [gravity.x, gravity.y, gravity.z]);

    const nativeRotation = new NativeQuaternion(wasmModule, nativeRotationPtr, true);
    const rotation = Quaternion.fromNative(nativeRotation);
    nativeRotation.close();
    return rotation;
  }

  static rotateVector3ByQuaternion(wasmModule: any, quat: Quaternion, vec3: Vector3): Vector3 {
    const nativeRotatedPtr = wasmModule.ccall('rgbd_math_utils_rotate_vector3_by_quaternion',
                                              'number',
                                              ['number', 'number', 'number', 'number', 'number', 'number', 'number'],
                                              [quat.w, quat.x, quat.y, quat.z, vec3.x, vec3.y, vec3.z]);

    const nativeRotated = new NativeVector3(wasmModule, nativeRotatedPtr, true);
    const rotated = Vector3.fromNative(nativeRotated);
    nativeRotated.close();
    return rotated;
  }

  static convertEulerAnglesToQuaternion(wasmModule: any, eulerAngles: Vector3): Quaternion {
    const nativeRotationPtr = wasmModule.ccall('rgbd_math_utils_convert_euler_angles_to_quaternion',
                                               'number',
                                               ['number', 'number', 'number'],
                                               [eulerAngles.x, eulerAngles.y, eulerAngles.z]);

    const nativeRotation = new NativeQuaternion(wasmModule, nativeRotationPtr, true);
    const rotation = Quaternion.fromNative(nativeRotation);
    nativeRotation.close();
    return rotation;
  }

  static multiplyQuaternions(wasmModule: any, quat1: Quaternion, quat2: Quaternion): Quaternion {
    const nativeRotationPtr = wasmModule.ccall('rgbd_math_utils_multiply_quaternions',
                                               'number',
                                               ['number', 'number', 'number', 'number',
                                                'number', 'number', 'number', 'number'],
                                               [quat1.w, quat1.x, quat1.y, quat1.z,
                                                quat2.w, quat2.x, quat2.y, quat2.z]);

    const nativeRotation = new NativeQuaternion(wasmModule, nativeRotationPtr, true);
    const rotation = Quaternion.fromNative(nativeRotation);
    nativeRotation.close();
    return rotation;
  }

  static extractYaw(wasmModule: any, quat: Quaternion): number {
    const yaw = wasmModule.ccall('rgbd_math_utils_extract_yaw',
                                 'number',
                                 ['number', 'number', 'number', 'number'],
                                 [quat.w, quat.x, quat.y, quat.z]);
    return yaw;
  }
}
