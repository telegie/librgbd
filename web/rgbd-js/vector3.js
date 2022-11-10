export class Vector3 {
  constructor(x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }

  multiply(factor) {
    return new Vector3(this.x * factor, this.y * factor, this.z * factor);
  }
}
