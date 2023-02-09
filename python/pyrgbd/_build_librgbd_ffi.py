import platform
from cffi import FFI
from pathlib import Path


ffi = FFI()
here = Path(__file__).resolve().parent
librgbd_root = here.parent.parent

if platform.system() == "Windows":
    extra_link_args = None
elif platform.system() == "Darwin":
    # TODO: These rpath to absolute library paths won't work
    # when the package is installed to another computer
    # without the binary file at the absolute rpath.
    extra_link_args = [f"-Wl,-rpath,@loader_path,-no_fixup_chains"]
    # extra_link_args = None
elif platform.system() == "Linux":
    include_dir = f"{librgbd_root}/include"
    extra_link_args = [f"-Wl,-rpath,$ORIGIN"]
else:
    raise Exception(f"Unknown platform.system(): {platform.system()}")

ffi.set_source(
    "_librgbd_ffi",
    r"#include <rgbd/rgbd_capi.h>",
    include_dirs=[f"{librgbd_root}/include"],
    libraries=["rgbd"],
    library_dirs=[f"{here}"],
    extra_link_args=extra_link_args,
)


# Use rgbd_capi.h except for the lines that cffi cannot handle.
cdef_lines = []
inside_cplusplus = False
with open(f"{librgbd_root}/include/rgbd/rgbd_capi.h") as f:
    lines = f.readlines()
    for line in lines:
        # Ignore lines only for when __cplusplus is defined.
        if inside_cplusplus:
            if line.startswith("#endif"):
                inside_cplusplus = False
            else:
                continue
        if line.startswith("#ifdef __cplusplus"):
            inside_cplusplus = True
        # Ignore the directives as cffi cannot handle them.
        if line.startswith("#"):
            continue
        # Replace RGBD_INTERFACE_EXPORT, which is added for exporting functions to DLL in windows.
        line = line.replace("RGBD_INTERFACE_EXPORT", "")
        cdef_lines.append(line)

ffi.cdef("".join(cdef_lines))


if __name__ == "__main__":
    here = Path(__file__).resolve().parent
    ffi.compile(tmpdir=str(here))
