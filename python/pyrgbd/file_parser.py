from ._librgbd_ffi import lib
from . import file as rgbd_file


class NativeFileParser:
    def __init__(self, file_path):
        self.ptr = lib.rgbd_file_parser_ctor_from_path(file_path.encode("utf8"))

    def close(self):
        lib.rgbd_file_parser_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def parse(self, with_frames: bool, with_directions: bool) -> rgbd_file.NativeFile:
        return rgbd_file.NativeFile(
            lib.rgbd_file_parser_parse(self.ptr, with_frames, with_directions)
        )
