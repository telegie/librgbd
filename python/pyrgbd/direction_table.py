import glm
import numpy as np
from ._librgbd_ffi import lib


class NativeDirectionTable:
    def __init__(self, ptr, owner: bool):
        self.ptr = ptr
        self.owner = owner

    def close(self):
        if self.owner:
            lib.rgbd_direction_table_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_width(self) -> int:
        print(f"self.ptr: {self.ptr}")
        return lib.rgbd_direction_table_get_width(self.ptr)

    def get_height(self) -> int:
        return lib.rgbd_direction_table_get_height(self.ptr)

    def get_direction_count(self) -> int:
        return lib.rgbd_direction_table_get_direction_count(self.ptr)

    def get_direction(self, index: int) -> glm.vec3:
        x = lib.rgbd_direction_table_get_direction_x(self.ptr, index)
        y = lib.rgbd_direction_table_get_direction_y(self.ptr, index)
        z = lib.rgbd_direction_table_get_direction_z(self.ptr, index)
        return glm.vec3(x, y, z)


class DirectionTable:
    def __init__(self, width: int, height: int, directions: list[glm.vec3]):
        self.width = width
        self.height = height
        self.directions = directions

    @classmethod
    def from_native(cls, native_direction_table: NativeDirectionTable):
        width = native_direction_table.get_width()
        height = native_direction_table.get_height()
        direction_count = native_direction_table.get_direction_count()
        directions = []
        for i in range(direction_count):
            directions.append(native_direction_table.get_direction(i))
        directions = directions
        return DirectionTable(width, height, directions)

    def to_np_array(self) -> np.array:
        directions = list(map(lambda v: v.to_list(), self.directions))
        return np.array(directions).reshape((self.height, self.width, 3))
