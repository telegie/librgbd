#!/usr/bin/env python3
import argparse
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path


def build_x64_windows_binaries():
    here = Path(__file__).parent.resolve()
    build_path = f"{here}/build/x64-windows"
    if not os.path.exists(build_path):
        os.makedirs(build_path)

    subprocess.run(["cmake",
                    "-S", here,
                    "-A" "x64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/x64-windows"],
                   cwd=build_path,
                   check=True)
    subprocess.run(["msbuild",
                    "INSTALL.vcxproj",
                    "/p:Configuration=RelWithDebInfo"],
                   cwd=build_path,
                   check=True)


def build_arm64_mac_binaries(disable_pybind):
    here = Path(__file__).parent.resolve()
    build_path = f"{here}/build/arm64-mac"

    cmake_args = []
    if disable_pybind:
        cmake_args += ["-DNO_PYBIND=ON"]

    subprocess.run(["cmake",
                    "-S", here,
                    "-B", build_path,
                    "-G", "Ninja",
                    "-D", "CMAKE_OSX_ARCHITECTURES=arm64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/arm64-mac"] + cmake_args,
                   check=True)
    subprocess.run(["ninja"], cwd=build_path, check=True)
    subprocess.run(["ninja", "install"], cwd=build_path, check=True)


def build_x64_mac_binaries():
    here = Path(__file__).parent.resolve()
    build_path = f"{here}/build/x64-mac"

    subprocess.run(["cmake",
                    "-S", here,
                    "-B", build_path,
                    "-G", "Ninja",
                    "-D", "CMAKE_OSX_ARCHITECTURES=x86_64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/x64-mac"],
                   check=True)
    subprocess.run(["ninja"], cwd=build_path, check=True)
    subprocess.run(["ninja", "install"], cwd=build_path, check=True)


def merge_mac_binaries():
    here = Path(__file__).parent.resolve()
    arm64_mac_output_path = f"{here}/output/arm64-mac"
    x64_mac_output_path = f"{here}/output/x64-mac"
    universal_mac_output_bin_path = f"{here}/output/universal-mac/bin"
    if not os.path.exists(universal_mac_output_bin_path):
        os.makedirs(universal_mac_output_bin_path)

    subprocess.run(["lipo",
                    "-create",
                    f"{arm64_mac_output_path}/bin/librgbd.dylib",
                    f"{x64_mac_output_path}/bin/librgbd.dylib",
                    "-output",
                    f"{universal_mac_output_bin_path}/librgbd.dylib"],
                   check=True)


def build_x64_linux_binaries(disable_pybind):
    here = Path(__file__).parent.resolve()
    build_path = f"{here}/build/x64-linux"

    cmake_args = []
    if disable_pybind:
        cmake_args += ["-DNO_PYBIND=ON"]

    subprocess.run(["cmake",
                    "-S", here,
                    "-B", build_path,
                    "-G", "Ninja",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/x64-linux"] + cmake_args)
    subprocess.run(["ninja"], cwd=build_path, check=True)
    subprocess.run(["ninja", "install"], cwd=build_path, check=True)




def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--rebuild", action="store_true")
    parser.add_argument("--disable-pybind", action="store_true")
    parser_args, _ = parser.parse_known_args()

    here = Path(__file__).parent.resolve()
    subprocess.run(["python3", f"{here}/bootstrap.py"] + sys.argv[1:], check=True)
    build_path = Path(f"{here}/build")
    if parser_args.rebuild and build_path.exists():
        shutil.rmtree(build_path)

    if platform.system() == "Windows":
        build_x64_windows_binaries()
        return
    elif platform.system() == "Darwin":
        if platform.machine() == "arm64":
            build_arm64_mac_binaries(parser_args.disable_pybind)
            build_x64_mac_binaries()
            merge_mac_binaries()
        elif platform.machine() == "x86_64":
            build_x64_mac_binaries()
        return
    elif platform.system() == "Linux":
        build_x64_linux_binaries(parser_args.disable_pybind)
        return

    raise Exception(f"librgbd build not supported.")


if __name__ == "__main__":
	main()
