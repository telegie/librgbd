export class VideoFrame {
  constructor(timePointUs, yuvFrame, depthFrame, depthConfidenceFrame, floor) {
    this.timePointUs = timePointUs;
    this.yuvFrame = yuvFrame;
    this.depthFrame = depthFrame;
    this.depthConfidenceFrame = depthConfidenceFrame;
    this.floor = floor;
  }
}
