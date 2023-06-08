export class AudioFrame {
  timePointUs: number;
  pcmSamples: number[];
  constructor(timePointUs: number, pcmSamples: number[]) {
    this.timePointUs = timePointUs;
    this.pcmSamples = pcmSamples;
  }
}
