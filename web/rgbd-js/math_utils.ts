import { Quaternion, Vector3 } from "@math.gl/core";
import { NativeQuaternion, NativeVector3 } from "./capi_containers"
import { EmscriptenModuleEx } from "./emscripten_module_ex";

export class MathUtils {
  static applyRotationRateAndGravityToRotation(module: EmscriptenModuleEx,
                                               rotation: Quaternion,                               
                                               deltaTimeSec: number,
                                               rotationRate: Vector3,
                                               gravity: Vector3): Quaternion {
    const nativeRotation = NativeQuaternion.fromMathGL(module, rotation);
    const nativeRotationRate = NativeVector3.fromMathGL(module, rotationRate);
    const nativeGravity = NativeVector3.fromMathGL(module, gravity);
    const nativeNewRotationPtr = module.ccall('rgbd_math_utils_apply_rotation_rate_and_gravity_to_rotation',
                                              'number',
                                              ['number', 'number', 'number', 'number'],
                                              [nativeRotation.getPtr(), deltaTimeSec, nativeRotationRate.getPtr(), nativeGravity.getPtr()]);
    nativeRotation.close();
    nativeRotationRate.close();
    nativeGravity.close();

    const nativeNewRotation = new NativeQuaternion(module, nativeNewRotationPtr, true);
    const newRotation = nativeNewRotation.toMathGL();
    nativeNewRotation.close();
    return newRotation;
  }

  static convertEulerAnglesToQuaternion(module: EmscriptenModuleEx, eulerAngles: Vector3): Quaternion {
    const nativeEulerAngles = NativeVector3.fromMathGL(module, eulerAngles);
    const nativeQuaternionPtr = module.ccall('rgbd_math_utils_convert_euler_angles_to_quaternion',
                                             'number',
                                             ['number'],
                                             [nativeEulerAngles.getPtr()]);
    nativeEulerAngles.close();

    const nativeQuaternion = new NativeQuaternion(module, nativeQuaternionPtr, true);
    const quat = nativeQuaternion.toMathGL();
    nativeQuaternion.close();
    return quat;
  }

  static convertQuaternionToEulerAngles(module: EmscriptenModuleEx, quat: Quaternion): Vector3 {
    const nativeQuaternion = NativeQuaternion.fromMathGL(module, quat);
    const nativeEulerAnglesPtr = module.ccall('rgbd_math_utils_convert_quaternion_to_euler_angles',
                                              'number',
                                              ['number'],
                                              [nativeQuaternion.getPtr()]);
    nativeQuaternion.close();

    const nativeEulerAngles = new NativeVector3(module, nativeEulerAnglesPtr, true);
    const eulerAngles = nativeEulerAngles.toMathGL();
    nativeEulerAngles.close();
    return eulerAngles;
  }

  static convertRGBToYuv420(module: EmscriptenModuleEx, width: number, height: number,
                            rChannel: Uint8Array, gChannel: Uint8Array, bChannel: Uint8Array): [Uint8Array, Uint8Array, Uint8Array] {
    const yChannel = new Uint8Array(width * height);
    const uChannel = new Uint8Array(width * height / 4);
    const vChannel = new Uint8Array(width * height / 4);
                       
    const rChannelPtr = module._malloc(rChannel.byteLength);
    const gChannelPtr = module._malloc(gChannel.byteLength);
    const bChannelPtr = module._malloc(bChannel.byteLength);
    const yChannelPtr = module._malloc(yChannel.byteLength);
    const uChannelPtr = module._malloc(uChannel.byteLength);
    const vChannelPtr = module._malloc(vChannel.byteLength);

    module.HEAPU8.set(rChannel, rChannelPtr);
    module.HEAPU8.set(gChannel, gChannelPtr);
    module.HEAPU8.set(bChannel, bChannelPtr);

    module.ccall('rgbd_math_utils_convert_rgb_to_yuv420',
                 null,
                 ['number', 'number',
                  'number', 'number', 'number',
                  'number', 'number', 'number'],
                 [width, height,
                  rChannelPtr, gChannelPtr, bChannelPtr,
                  yChannelPtr, uChannelPtr, vChannelPtr]);

    // reference: https://github.com/emscripten-core/emscripten/issues/7965
    yChannel.set(module.HEAPU8.subarray(yChannelPtr, yChannelPtr + yChannel.byteLength));
    uChannel.set(module.HEAPU8.subarray(uChannelPtr, uChannelPtr + uChannel.byteLength));
    vChannel.set(module.HEAPU8.subarray(vChannelPtr, vChannelPtr + vChannel.byteLength));

    module._free(rChannelPtr);
    module._free(gChannelPtr);
    module._free(bChannelPtr);
    module._free(yChannelPtr);
    module._free(uChannelPtr);
    module._free(vChannelPtr);

    return [yChannel, uChannel, vChannel];
  }
}
