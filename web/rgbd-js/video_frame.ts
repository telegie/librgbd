import { YuvFrame } from  "./yuv_frame";
import { Int32Frame } from  "./integer_frame";

export class VideoFrame {
  timePointUs: number;
  yuvFrame: YuvFrame;
  depthFrame: Int32Frame;

  constructor(timePointUs: number, yuvFrame: YuvFrame, depthFrame: Int32Frame) {
    this.timePointUs = timePointUs;
    this.yuvFrame = yuvFrame;
    this.depthFrame = depthFrame;
  }
}
