from cffi import FFI
from pathlib import Path
import os
import platform
import shutil
import subprocess
import sys


def compile_with_cffi():
    here = Path(__file__).parent.resolve()
    root = Path(__file__).parent.parent.resolve()
    ffi = FFI()
    if platform.system() == "Windows":
        librgbd_path = f"{root}/output/x64-windows"
        librgbd_include_dir = f"{librgbd_path}/include"
        library_str = "rgbd"
        librgbd_library_dir = f"{librgbd_path}/bin"

        ffi.set_source('_librgbd',
                       r'#include <rgbd/rgbd_capi.h>',
                       include_dirs=[str(librgbd_include_dir)],
                       libraries=[library_str],
                       library_dirs=[str(librgbd_library_dir)])
    elif platform.system() == "Darwin":
        if platform.machine() == "arm64":
            librgbd_path = f"{root}/output/arm64-mac"
        elif platform.machine() == "x86_64":
            librgbd_path = f"{root}/output/x64-mac"
        else:
            raise f"Unknown platform.machine(): {platform.machine()}"

        librgbd_include_dir = f"{librgbd_path}/include"
        library_str = "rgbd"
        librgbd_library_dir = f"{librgbd_path}/bin"
        # Add same directory in rpath to find the dylib in the same directory.
        extra_link_args_str = f"-Wl,-rpath,{here}/pyrgbd"

        ffi.set_source('_librgbd',
                       r'#include <rgbd/rgbd_capi.h>',
                       include_dirs=[str(librgbd_include_dir)],
                       libraries=[library_str],
                       library_dirs=[str(librgbd_library_dir)],
                       extra_link_args=[extra_link_args_str])
    elif platform.system() == "Linux":
        librgbd_path = f"{root}/output/x64-linux"
        librgbd_include_dir = f"{librgbd_path}/include"
        library_str = "rgbd"
        librgbd_library_dir = f"{librgbd_path}/bin"
        extra_link_args_str = f"-Wl,-rpath,{here}/pyrgbd"

        ffi.set_source('_librgbd',
                       r'#include <rgbd/rgbd_capi.h>',
                       include_dirs=[str(librgbd_include_dir)],
                       libraries=[library_str],
                       library_dirs=[str(librgbd_library_dir)],
                       extra_link_args=[extra_link_args_str])
    else:
        raise f"Unknown platform.system(): {platform.system()}"


    cdef_lines = []
    inside_cplusplus = False
    with open(os.path.join(librgbd_include_dir, "rgbd/rgbd_capi.h")) as f:
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
    ffi.compile(tmpdir=f"{here}/pyrgbd")


def copy_binaries():
    here = Path(__file__).parent.resolve()
    root = Path(__file__).parent.parent.resolve()

    if platform.system() == "Windows":
        librgbd_dll_dirs = [f"{root}/output/x64-windows/bin"]
        librgbd_dll_filenames = ["rgbd.dll"]

        ffmpeg_binaries_dir = f"{root}/deps/ffmpeg-binaries"
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
            destination = f"{here}\\pyrgbd\\{dll_filename}"
            if os.path.exists(destination):
                os.remove(destination)
            shutil.copy(f"{dll_dir}\\{dll_filename}", destination)
    elif platform.system() == "Darwin":
        if platform.machine() == "arm64":
            librgbd_bin_dir = f"{root}/output/arm64-mac/bin"
        elif platform.machine() == "x86_64":
            librgbd_bin_dir = f"{root}/output/x64-mac/bin"
        else:
            raise f"Unknown platform.machine(): {platform.machine()}"
        destination = f"{here}/pyrgbd/librgbd.dylib"
        # Should remove the existing one before copying.
        # Simply copying does not overwrite properly.
        if os.path.exists(destination):
            os.remove(destination)
        shutil.copy(f"{librgbd_bin_dir}/librgbd.dylib", destination)
    elif platform.system() == "Linux":
        librgbd_bin_dir = f"{root}/output/x64-linux/bin"
        destination = f"{here}/pyrgbd/librgbd.so"
        # Should remove the existing one before copying.
        # Simply copying does not overwrite properly.
        if os.path.exists(destination):
            os.remove(destination)
        shutil.copy(f"{librgbd_bin_dir}/librgbd.so", destination)
    else:
        raise f"Unknown platform.system(): {platform.system()}"


def main():
    root = Path(__file__).parent.parent.resolve()
    args = ["python3", f"{root}/build.py"]
    args += sys.argv[1:]
    subprocess.run(args, check=True)
    print("build_librgbd done")
    compile_with_cffi()
    print("compile_with_cffi done")
    copy_binaries()
    print("copy_binaries done")


if __name__ == "__main__":
    main()
