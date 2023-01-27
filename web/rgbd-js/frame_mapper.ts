import { CameraCalibration } from "./camera_calibration";
import { NativeInt32Frame, Int32Frame } from "./integer_frame";
import { NativeYuvFrame, YuvFrame } from "./yuv_frame";

export class NativeFrameMapper {
  wasmModule: any;
  ptr: number;

  constructor(wasmModule: any, srcCalibration: CameraCalibration, dstCalibration: CameraCalibration) {
    this.wasmModule = wasmModule;
    const nativeSrcCalibration = srcCalibration.toNative(wasmModule);
    const nativeDstCalibration = dstCalibration.toNative(wasmModule);
    this.ptr = this.wasmModule.ccall('rgbd_frame_mapper_ctor',
                                      'number',
                                      ['number', 'number'],
                                      [nativeSrcCalibration.ptr, nativeDstCalibration.ptr]);
    nativeSrcCalibration.close();
    nativeDstCalibration.close();
  }

  close() {
    this.wasmModule.ccall('rgbd_frame_mapper_dtor', null, ['number'], [this.ptr]);
  }

  mapColorFrame(colorFrame: YuvFrame): YuvFrame {
    const nativeColorFrame = colorFrame.toNative(this.wasmModule);
    const mappedColorFramePtr = this.wasmModule.ccall('rgbd_frame_mapper_map_color_frame',
                                                      'number',
                                                      ['number', 'number'],
                                                      [this.ptr, nativeColorFrame.ptr]);
    nativeColorFrame.close();
    
    const nativeMappedColorFrame = new NativeYuvFrame(this.wasmModule, mappedColorFramePtr);
    const mappedColorFrame = YuvFrame.fromNative(nativeMappedColorFrame);
    nativeMappedColorFrame.close();
    return mappedColorFrame;
  }

  mapDepthFrame(depthFrame: Int32Frame): Int32Frame {
    const nativeDepthFrame = depthFrame.toNative(this.wasmModule);
    const mappedDepthFramePtr = this.wasmModule.ccall('rgbd_frame_mapper_map_depth_frame',
                                                      'number',
                                                      ['number', 'number'],
                                                      [this.ptr, nativeDepthFrame.ptr]);
    nativeDepthFrame.close();
    
    const nativeMappedDepthFrame = new NativeInt32Frame(this.wasmModule, mappedDepthFramePtr);
    const mappedDepthFrame = Int32Frame.fromNative(nativeMappedDepthFrame);
    nativeMappedDepthFrame.close();
    return mappedDepthFrame;
  }
}
  