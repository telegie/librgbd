import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path


def copy_librgbd_files():
    here = Path(__file__).resolve().parent
    src = f"{here.parent}/output"
    dst = f"{here}/pyrgbd/_librgbd"
    if os.path.exists(dst):
        shutil.rmtree(dst)
    shutil.copytree(src, dst)

    # TODO: Copy these FFmpeg files into the Windows package.
    # if platform.system() == "Windows":
    #     librgbd_dll_dirs = [f"{librgbd_root}/output/x64-windows/bin"]
    #     librgbd_dll_filenames = ["rgbd.dll"]
    #
    #     ffmpeg_binaries_dir = f"{librgbd_root}/deps/ffmpeg-binaries"
    #     librgbd_dll_dirs.append(f"{ffmpeg_binaries_dir}/bin")
    #     librgbd_dll_filenames.append("libwinpthread-1.dll")
    #     librgbd_dll_dirs.append(f"{ffmpeg_binaries_dir}/bin")
    #     librgbd_dll_filenames.append("zlib1.dll")
    #     librgbd_dll_dirs.append(f"{ffmpeg_binaries_dir}/4.4.1/x64-windows/bin")
    #     librgbd_dll_filenames.append("avcodec-58.dll")
    #     librgbd_dll_dirs.append(f"{ffmpeg_binaries_dir}/4.4.1/x64-windows/bin")
    #     librgbd_dll_filenames.append("avutil-56.dll")
    #     for index in range(len(librgbd_dll_dirs)):
    #         dll_dir = librgbd_dll_dirs[index]
    #         dll_filename = librgbd_dll_filenames[index]
    #         destination = f"{here}/pyrgbd/_librgbd{dll_filename}"
    #         if os.path.exists(destination):
    #             os.remove(destination)
    #         shutil.copy(f"{dll_dir}/{dll_filename}", destination)


def main():
    here = Path(__file__).parent.resolve()
    librgbd_root = Path(__file__).parent.parent.resolve()
    subprocess.run(["python3", f"{librgbd_root}/build.py"] + sys.argv[1:], check=True)
    copy_librgbd_files()
    subprocess.run(["python3", f"{here}/pyrgbd/_librgbd_ffi.py"] + sys.argv[1:], check=True)


if __name__ == "__main__":
    main()
