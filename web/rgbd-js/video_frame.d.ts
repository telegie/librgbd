import { Int32Frame } from './interger_frame';
import { YuvFrame } from './yuv_frame';

export declare class VideoFrame {
  timePointUs: number;
  yuvFrame: YuvFrame;
  depthFrame: Int32Frame;
  floor: Plane;

  constructor(timePointUs: number, yuvFrame: YuvFrame, depthFrame: Int32Frame, floor: Plane);
}
