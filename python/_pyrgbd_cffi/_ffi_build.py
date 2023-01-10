import os
import platform
import shutil
import subprocess
import sys
from cffi import FFI
from pathlib import Path


def compile_with_cffi():
    here = Path(__file__).parent.resolve()
    librgbd_root = Path(__file__).parent.parent.parent.resolve()

    ffi = FFI()
    if platform.system() == "Windows":
        librgbd_platform_output = f"{librgbd_root}/output/x64-windows"
        extra_link_args_str = ""
    elif platform.system() == "Darwin":
        if platform.machine() == "arm64":
            librgbd_platform_output = f"{librgbd_root}/output/arm64-mac"
        elif platform.machine() == "x86_64":
            librgbd_platform_output = f"{librgbd_root}/output/x64-mac"
        else:
            raise f"Unknown platform.machine(): {platform.machine()}"
        extra_link_args_str = f"-Wl,-rpath,{here}"
    elif platform.system() == "Linux":
        librgbd_platform_output = f"{librgbd_root}/output/x64-linux"
        extra_link_args_str = f"-Wl,-rpath,{here}"
    else:
        raise f"Unknown platform.system(): {platform.system()}"

    librgbd_include = f"{librgbd_platform_output}/include"
    ffi.set_source('_librgbd',
                   r'#include <rgbd/rgbd_capi.h>',
                   include_dirs=[librgbd_include],
                   libraries=["rgbd"],
                   library_dirs=[f"{librgbd_platform_output}/bin"],
                   extra_link_args=[extra_link_args_str])

    cdef_lines = []
    inside_cplusplus = False
    with open(os.path.join(librgbd_include, "rgbd/rgbd_capi.h")) as f:
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
    # Output of the compilation goes to tmpdir.
    ffi.compile(tmpdir=f"{here}")


def copy_binaries():
    here = Path(__file__).parent.resolve()
    # root = Path(__file__).parent.parent.resolve()
    # pyrgbd_root = Path(__file__).parent.parent.resolve()
    librgbd_root = Path(__file__).parent.parent.parent.resolve()

    if platform.system() == "Windows":
        librgbd_dll_dirs = [f"{librgbd_root}/output/x64-windows/bin"]
        librgbd_dll_filenames = ["rgbd.dll"]

        ffmpeg_binaries_dir = f"{librgbd_root}/deps/ffmpeg-binaries"
        librgbd_dll_dirs.append(f"{ffmpeg_binaries_dir}/bin")
        librgbd_dll_filenames.append("libwinpthread-1.dll")
        librgbd_dll_dirs.append(f"{ffmpeg_binaries_dir}/bin")
        librgbd_dll_filenames.append("zlib1.dll")
        librgbd_dll_dirs.append(f"{ffmpeg_binaries_dir}/4.4.1/x64-windows/bin")
        librgbd_dll_filenames.append("avcodec-58.dll")
        librgbd_dll_dirs.append(f"{ffmpeg_binaries_dir}/4.4.1/x64-windows/bin")
        librgbd_dll_filenames.append("avutil-56.dll")
        for index in range(len(librgbd_dll_dirs)):
            dll_dir = librgbd_dll_dirs[index]
            dll_filename = librgbd_dll_filenames[index]
            destination = f"{here}\\{dll_filename}"
            if os.path.exists(destination):
                os.remove(destination)
            shutil.copy(f"{dll_dir}\\{dll_filename}", destination)
    elif platform.system() == "Darwin":
        if platform.machine() == "arm64":
            librgbd_bin_dir = f"{librgbd_root}/output/arm64-mac/bin"
        elif platform.machine() == "x86_64":
            librgbd_bin_dir = f"{librgbd_root}/output/x64-mac/bin"
        else:
            raise f"Unknown platform.machine(): {platform.machine()}"
        destination = f"{here}/librgbd.dylib"
        # Should remove the existing one before copying.
        # Simply copying does not overwrite properly.
        if os.path.exists(destination):
            os.remove(destination)
        shutil.copy(f"{librgbd_bin_dir}/librgbd.dylib", destination)
    elif platform.system() == "Linux":
        librgbd_bin_dir = f"{librgbd_root}/output/x64-linux/bin"
        destination = f"{here}/librgbd.so"
        # Should remove the existing one before copying.
        # Simply copying does not overwrite properly.
        if os.path.exists(destination):
            os.remove(destination)
        shutil.copy(f"{librgbd_bin_dir}/librgbd.so", destination)
    else:
        raise f"Unknown platform.system(): {platform.system()}"


def main():
    librgbd_root = Path(__file__).parent.parent.parent.resolve()
    args = ["python3", f"{librgbd_root}/build.py"]
    args += sys.argv[1:]
    subprocess.run(args, check=True)
    compile_with_cffi()
    copy_binaries()
    print("pyrgbd bootstrapped!")


if __name__ == "__main__":
    main()
