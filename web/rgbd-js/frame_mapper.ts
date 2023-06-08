import { CameraCalibration } from "./camera_calibration";
import { EmscriptenModuleEx } from "./emscripten_module_ex";
import { NativeInt32Frame, Int32Frame } from "./integer_frame";
import { NativeObject } from "./native_object";
import { NativeYuvFrame, YuvFrame } from "./yuv_frame";

export class NativeFrameMapper extends NativeObject {
  constructor(module: EmscriptenModuleEx, srcCalibration: CameraCalibration, dstCalibration: CameraCalibration) {
    const nativeSrcCalibration = srcCalibration.toNative(module);
    const nativeDstCalibration = dstCalibration.toNative(module);
    const ptr = module.ccall('rgbd_frame_mapper_ctor',
                             'number',
                             ['number', 'number'],
                             [nativeSrcCalibration.getPtr(), nativeDstCalibration.getPtr()]);
    nativeSrcCalibration.close();
    nativeDstCalibration.close();
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_frame_mapper_dtor', null, ['number'], [this.getPtr()]);
  }

  mapColorFrame(colorFrame: YuvFrame): YuvFrame {
    const nativeColorFrame = colorFrame.toNative(this.getModule());
    const mappedColorFramePtr = this.getModule().ccall('rgbd_frame_mapper_map_color_frame',
                                                  'number',
                                                  ['number', 'number'],
                                                  [this.getPtr(), nativeColorFrame.getPtr()]);
    nativeColorFrame.close();
    
    const nativeMappedColorFrame = new NativeYuvFrame(this.getModule(), mappedColorFramePtr);
    const mappedColorFrame = YuvFrame.fromNative(nativeMappedColorFrame);
    nativeMappedColorFrame.close();
    return mappedColorFrame;
  }

  mapDepthFrame(depthFrame: Int32Frame): Int32Frame {
    const nativeDepthFrame = depthFrame.toNative(this.getModule());
    const mappedDepthFramePtr = this.getModule().ccall('rgbd_frame_mapper_map_depth_frame',
                                                  'number',
                                                  ['number', 'number'],
                                                  [this.getPtr(), nativeDepthFrame.getPtr()]);
    nativeDepthFrame.close();
    
    const nativeMappedDepthFrame = new NativeInt32Frame(this.getModule(), mappedDepthFramePtr);
    const mappedDepthFrame = Int32Frame.fromNative(nativeMappedDepthFrame);
    nativeMappedDepthFrame.close();
    return mappedDepthFrame;
  }
}
  