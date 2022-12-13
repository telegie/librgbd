#!/usr/bin/env python3
import os
import platform
import subprocess
from pathlib import Path


def build_x64_windows_binaries():
    here = Path(__file__).parent.resolve()
    build_path = f"{here}/build/x64-windows"

    subprocess.run(["cmake",
                    "-S", here,
                    "-B", build_path,
                    "-A" "x64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/install/x64-windows"],
                   check=True)
    subprocess.run(["msbuild",
                    f"{build_path}/INSTALL.vcxproj",
                    "/p:Configuration=RelWithDebInfo"],
                   check=True)


def build_arm64_mac_binaries():
    here = Path(__file__).parent.resolve()
    build_path = f"{here}/build/arm64-mac"

    subprocess.run(["cmake",
                    "-S", here,
                    "-B", build_path,
                    "-G", "Ninja",
                    "-D", "CMAKE_OSX_ARCHITECTURES=arm64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/install/arm64-mac"],
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
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/install/x64-mac"],
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
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/install/x64-linux"])
    subprocess.run(["ninja"], cwd=build_path, check=True)
    subprocess.run(["ninja", "install"], cwd=build_path, check=True)


def main():
    here = Path(__file__).parent.resolve()
    subprocess.run(["python3", f"{here}/bootstrap.py"], check=True)

    if platform.system() == "Windows":
        build_x64_windows_binaries()
        return
    elif platform.system() == "Darwin":
        build_arm64_mac_binaries()
        build_x64_mac_binaries()
        return
    elif platform.system() == "Linux":
        build_x64_linux_binaries()
        return

    raise Exception(f"librgbd build not supported.")


if __name__ == "__main__":
	main()
