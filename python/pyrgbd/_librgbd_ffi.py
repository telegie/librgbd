import os
import platform
from cffi import FFI
from pathlib import Path


ffi = FFI()
here = Path(__file__).resolve().parent

if platform.system() == "Windows":
    copied_librgbd_dir = f"{here}/_librgbd/x64-windows"
    extra_link_args_str = ""
elif platform.system() == "Darwin":
    if platform.machine() == "arm64":
        copied_librgbd_dir = f"{here}/_librgbd/arm64-mac"
    elif platform.machine() == "x86_64":
        copied_librgbd_dir = f"{here}/_librgbd/x64-mac"
    else:
        raise f"Unknown platform.machine(): {platform.machine()}"
    extra_link_args_str = f"-Wl,-rpath,{here}"
elif platform.system() == "Linux":
    copied_librgbd_dir = f"{here}/_librgbd/x64-linux"
    extra_link_args_str = f"-Wl,-rpath,{here}"
else:
    raise f"Unknown platform.system(): {platform.system()}"

ffi.set_source('_librgbd',
               r'#include <rgbd/rgbd_capi.h>',
               include_dirs=[f"{copied_librgbd_dir}/include"],
               libraries=["rgbd"],
               library_dirs=[f"{copied_librgbd_dir}/bin"],
               extra_link_args=[extra_link_args_str])


# Use rgbd_capi.h except for the lines that cffi cannot handle.
cdef_lines = []
inside_cplusplus = False
with open(os.path.join(f"{copied_librgbd_dir}", "include/rgbd/rgbd_capi.h")) as f:
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
    ffi.compile()
