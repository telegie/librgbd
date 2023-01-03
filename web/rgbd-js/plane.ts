import { Vector3 } from "./vector3";

export class Plane {
  normal: Vector3;
  constant: number;

  constructor(normal: Vector3, constant: number) {
    this.normal = normal;
    this.constant = constant;
  }
} 