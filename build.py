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


def build_arm64_mac_binaries():
    here = Path(__file__).parent.resolve()
    build_path = f"{here}/build/arm64-mac"

    subprocess.run(["cmake",
                    "-S", here,
                    "-B", build_path,
                    "-G", "Ninja",
                    "-D", "CMAKE_OSX_ARCHITECTURES=arm64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/arm64-mac"],
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


def build_x64_linux_binaries():
    here = Path(__file__).parent.resolve()
    build_path = f"{here}/build/x64-linux"

    subprocess.run(["cmake",
                    "-S", here,
                    "-B", build_path,
                    "-G", "Ninja",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/x64-linux"])
    subprocess.run(["ninja"], cwd=build_path, check=True)
    subprocess.run(["ninja", "install"], cwd=build_path, check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--rebuild", action="store_true")
    parser_args = parser.parse_args()

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
            build_arm64_mac_binaries()
            build_x64_mac_binaries()
        elif platform.machine() == "x86_64":
            build_x64_mac_binaries()
        return
    elif platform.system() == "Linux":
        build_x64_linux_binaries()
        return

    raise Exception(f"librgbd build not supported.")


if __name__ == "__main__":
	main()
