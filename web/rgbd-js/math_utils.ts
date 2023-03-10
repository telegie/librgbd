import RGBD from "../rgbd";

export class MathUtils {
  static computeGravityCompensatingEulerAngles(wasmModule: any, gravity: RGBD.Vector3): RGBD.Vector3 {
    const nativeEulerAnglesPtr = wasmModule.ccall('rgbd_math_utils_compute_gravity_compensating_euler_angles',
                                                  'number',
                                                  ['number', 'number', 'number'],
                                                  [gravity.x, gravity.y, gravity.z]);

    const nativeFloatArray = new RGBD.NativeFloatArray(wasmModule, nativeEulerAnglesPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    const eulerAngles = new RGBD.Vector3(floatArray[0], floatArray[1], floatArray[2]);
    return eulerAngles;
  }

  static computeGravityCompensatingRotation(wasmModule: any, gravity: RGBD.Vector3): RGBD.Quaternion {
    const nativeRotationPtr = wasmModule.ccall('rgbd_math_utils_compute_gravity_compensating_rotation',
                                               'number',
                                               ['number', 'number', 'number'],
                                               [gravity.x, gravity.y, gravity.z]);

    const nativeFloatArray = new RGBD.NativeFloatArray(wasmModule, nativeRotationPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    const rotation = new RGBD.Quaternion(floatArray[0], floatArray[1], floatArray[2], floatArray[3]);
    return rotation;
  }

  static rotateVector3ByQuaternion(wasmModule: any, quat: RGBD.Quaternion, vec3: RGBD.Vector3): RGBD.Vector3 {
    const nativeRotatedPtr = wasmModule.ccall('rgbd_math_utils_rotate_vector3_by_quaternion',
                                              'number',
                                              ['number', 'number', 'number', 'number', 'number', 'number', 'number'],
                                              [quat.w, quat.x, quat.y, quat.z, vec3.x, vec3.y, vec3.z]);

    const nativeFloatArray = new RGBD.NativeFloatArray(wasmModule, nativeRotatedPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    const rotated = new RGBD.Vector3(floatArray[0], floatArray[1], floatArray[2]);
    return rotated;
  }

  static convertEulerAnglesToQuaternion(wasmModule: any, eulerAngles: RGBD.Vector3): RGBD.Quaternion {
    const nativeRotationPtr = wasmModule.ccall('rgbd_math_utils_convert_euler_angles_to_quaternion',
                                               'number',
                                               ['number', 'number', 'number'],
                                               [eulerAngles.x, eulerAngles.y, eulerAngles.z]);

    const nativeFloatArray = new RGBD.NativeFloatArray(wasmModule, nativeRotationPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    const rotation = new RGBD.Quaternion(floatArray[0], floatArray[1], floatArray[2], floatArray[3]);
    return rotation;
  }

  static multiplyQuaternions(wasmModule: any, quat1: RGBD.Quaternion, quat2: RGBD.Quaternion): RGBD.Quaternion {
    const nativeRotationPtr = wasmModule.ccall('rgbd_math_utils_multiply_quaternions',
                                               'number',
                                               ['number', 'number', 'number', 'number',
                                                'number', 'number', 'number', 'number'],
                                               [quat1.w, quat1.x, quat1.y, quat1.z,
                                                quat2.w, quat2.x, quat2.y, quat2.z]);

    const nativeFloatArray = new RGBD.NativeFloatArray(wasmModule, nativeRotationPtr);
    const floatArray = nativeFloatArray.toArray();
    nativeFloatArray.close();

    const rotation = new RGBD.Quaternion(floatArray[0], floatArray[1], floatArray[2], floatArray[3]);
    return rotation;
  }

  static extractYaw(wasmModule: any, quat: RGBD.Quaternion): number {
    const yaw = wasmModule.ccall('rgbd_math_utils_extract_yaw',
                                 'number',
                                 ['number', 'number', 'number', 'number'],
                                 [quat.w, quat.x, quat.y, quat.z]);
    return yaw;
  }
}
