import { YuvFrame } from  "./yuv_frame";
import { Int32Frame } from  "./integer_frame";
import { Plane } from  "./plane";

export class VideoFrame {
  timePointUs: number;
  yuvFrame: YuvFrame;
  depthFrame: Int32Frame;
  floor: Plane | null;

  constructor(timePointUs: number, yuvFrame: YuvFrame, depthFrame: Int32Frame, floor: Plane | null) {
    this.timePointUs = timePointUs;
    this.yuvFrame = yuvFrame;
    this.depthFrame = depthFrame;
    this.floor = floor;
  }
}
