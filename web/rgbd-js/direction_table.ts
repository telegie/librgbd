import { Vector3 } from "@math.gl/core";
import { NativeObject } from "./native_object";

export class NativeDirectionTable extends NativeObject {
  constructor(wasmModule: any, ptr: number, owner: boolean) {
    super(wasmModule, ptr, owner);
  }

  delete() {
    this.wasmModule.ccall('rgbd_direction_table_dtor', null, ['number'], [this.ptr]);
  }

  getWidth(): number {
    return this.wasmModule.ccall('rgbd_direction_table_get_width', 'number', ['number'], [this.ptr]);
  }

  getHeight(): number {
    return this.wasmModule.ccall('rgbd_direction_table_get_height', 'number', ['number'], [this.ptr]);
  }

  getDirectionCount(): number {
    return this.wasmModule.ccall('rgbd_direction_table_get_direction_count', 'number', ['number'], [this.ptr]);
  }

  getDirection(index: number): Vector3 {
    const x = this.wasmModule.ccall('rgbd_direction_table_get_direction_x',
                                    'number',
                                    ['number', 'number'],
                                    [this.ptr, index]);
    const y = this.wasmModule.ccall('rgbd_direction_table_get_direction_y',
                                    'number',
                                    ['number', 'number'],
                                    [this.ptr, index]);
    const z = this.wasmModule.ccall('rgbd_direction_table_get_direction_z',
                                    'number',
                                    ['number', 'number'],
                                    [this.ptr, index]);
    return new Vector3(x, y, z);
  }
}

export class DirectionTable {
  width: number;
  height: number;
  directions: Vector3[];

  constructor(width: number, height: number, directions: Vector3[]) {
    this.width = width;
    this.height = height;
    this.directions = directions;
  }

  static fromNative(nativeDirectionTable: NativeDirectionTable): DirectionTable {
    const width = nativeDirectionTable.getWidth();
    const height = nativeDirectionTable.getHeight();
    const directionCount = nativeDirectionTable.getDirectionCount();
    const directions = new Array<Vector3>();
    for (let i = 0; i < directionCount; i += 1)
      directions.push(nativeDirectionTable.getDirection(i));
    return new DirectionTable(width, height, directions);
  }
}
