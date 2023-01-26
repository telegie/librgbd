import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path


def copy_librgbd():
    here = Path(__file__).resolve().parent
    librgbd_root = here.parent

    if platform.system() == "Windows":
        srcs = [f"{librgbd_root}/output/x64-windows/bin/rgbd.dll",
                f"{librgbd_root}/output/x64-windows/bin/rgbd.lib"]
    elif platform.system() == "Darwin":
        if platform.machine() == "arm64":
            # srcs = [f"{librgbd_root}/output/arm64-mac/bin/librgbd.dylib"]
            srcs = [f"{librgbd_root}/output/universal-mac/bin/librgbd.dylib"]
        elif platform.machine() == "x86_64":
            srcs = [f"{librgbd_root}/output/x64-mac/bin/librgbd.dylib"]
        else:
            raise f"Unknown platform.machine(): {platform.machine()}"
    elif platform.system() == "Linux":
        srcs = [f"{librgbd_root}/output/x64-linux/bin/librgbd.so"]
    else:
        raise f"Unknown platform.system(): {platform.system()}"

    for src in srcs:
        shutil.copy(src, f"{here}/pyrgbd")


def copy_windows_dependencies():
    here = Path(__file__).resolve().parent
    librgbd_root = here.parent
    librgbd_dll_dirs = []
    librgbd_dll_filenames = []

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
        destination = f"{here}/pyrgbd/{dll_filename}"
        if os.path.exists(destination):
            os.remove(destination)
        shutil.copy(f"{dll_dir}/{dll_filename}", destination)


def main():
    here = Path(__file__).parent.resolve()
    librgbd_root = Path(__file__).parent.parent.resolve()
    subprocess.run(["python3", f"{librgbd_root}/build.py"] + sys.argv[1:], check=True)
    copy_librgbd()
    if platform.system() == "Windows":
        copy_windows_dependencies()
    subprocess.run(["pip3", "install", "cffi"], check=True)
    print("cffi installed")
    subprocess.run(["python3", f"{here}/pyrgbd/_build_librgbd_ffi.py"], check=True)
    print("_build_librgbd_ffi done")


if __name__ == "__main__":
    main()
