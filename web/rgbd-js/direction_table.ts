import { Vector3 } from "@math.gl/core";
import { NativeObject } from "./native_object";
import { EmscriptenModuleEx } from "./emscripten_module_ex";

export class NativeDirectionTable extends NativeObject {
  constructor(module: EmscriptenModuleEx, ptr: number, owner: boolean) {
    super(module, ptr, owner);
  }

  delete() {
    this.getModule().ccall('rgbd_direction_table_dtor', null, ['number'], [this.getPtr()]);
  }

  getWidth(): number {
    return this.getModule().ccall('rgbd_direction_table_get_width', 'number', ['number'], [this.getPtr()]);
  }

  getHeight(): number {
    return this.getModule().ccall('rgbd_direction_table_get_height', 'number', ['number'], [this.getPtr()]);
  }

  getDirectionCount(): number {
    return this.getModule().ccall('rgbd_direction_table_get_direction_count', 'number', ['number'], [this.getPtr()]);
  }

  getDirection(index: number): Vector3 {
    const x = this.getModule().ccall('rgbd_direction_table_get_direction_x',
                                'number',
                                ['number', 'number'],
                                [this.getPtr(), index]);
    const y = this.getModule().ccall('rgbd_direction_table_get_direction_y',
                                'number',
                                ['number', 'number'],
                                [this.getPtr(), index]);
    const z = this.getModule().ccall('rgbd_direction_table_get_direction_z',
                                'number',
                                ['number', 'number'],
                                [this.getPtr(), index]);
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
