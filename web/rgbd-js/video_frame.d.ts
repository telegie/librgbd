export declare class VideoFrame {
  timePointUs: number;
  yuvFrame: YuvFrame;
  depthFrame: Int32Frame;
  floor: Plane;

  constructor(timePointUs: number, yuvFrame: YuvFrame, depthFrame: Int32Frame, floor: Plane);
}
