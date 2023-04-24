#!/usr/bin/env python3
import argparse
import platform
import shutil
import subprocess
from pathlib import Path


def cleanup_zconf_h():
    here = Path(__file__).parent.resolve()
    # The line below is needed not to have zlib repository as modified,
    # which leaves an annoying message in git.
    shutil.move(f"{here}/zlib/zconf.h.included", f"{here}/zlib/zconf.h")


def build_x64_windows_binaries():
    here = Path(__file__).parent.resolve()
    subprocess.run(["cmake",
                    "-S", f"{here}/zlib",
                    "-B", f"{here}/build/x64-windows",
                    "-A" "x64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/x64-windows"],
                   check=True)
    subprocess.run(["msbuild",
                    f"{here}/build/x64-windows/INSTALL.vcxproj",
                    "/p:Configuration=RelWithDebInfo"],
                   check=True)
    cleanup_zconf_h()


def build_arm64_mac_binaries():
    here = Path(__file__).parent.resolve()
    subprocess.run(["cmake",
                    "-S", f"{here}/zlib",
                    "-B", f"{here}/build/arm64-mac",
                    "-G", "Ninja",
                    "-D", "CMAKE_OSX_ARCHITECTURES=arm64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/arm64-mac"],
                   check=True)
    subprocess.run(["ninja"], cwd=f"{here}/build/arm64-mac", check=True)
    subprocess.run(["ninja", "install"], cwd=f"{here}/build/arm64-mac", check=True)
    cleanup_zconf_h()


def build_x64_mac_binaries():
    here = Path(__file__).parent.resolve()
    subprocess.run(["cmake",
                    "-S", f"{here}/zlib",
                    "-B", f"{here}/build/x64-mac",
                    "-G", "Ninja",
                    "-D", "CMAKE_OSX_ARCHITECTURES=x86_64",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/x64-mac"],
                   check=True)
    subprocess.run(["ninja"], cwd=f"{here}/build/x64-mac", check=True)
    subprocess.run(["ninja", "install"], cwd=f"{here}/build/x64-mac", check=True)
    cleanup_zconf_h()


def build_x64_linux_binaries():
    here = Path(__file__).parent.resolve()
    subprocess.run(["cmake",
                    "-S", f"{here}/zlib",
                    "-B", f"{here}/build/x64-linux",
                    "-G", "Ninja",
                    "-D", f"CMAKE_INSTALL_PREFIX={here}/output/x64-linux",
                    "-D", "CMAKE_C_FLAGS=-fPIC"],
                   check=True)
    subprocess.run(["ninja"], cwd=f"{here}/build/x64-linux", check=True)
    subprocess.run(["ninja", "install"], cwd=f"{here}/build/x64-linux", check=True)
    cleanup_zconf_h()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--rebuild", action="store_true")
    parser_args, _ = parser.parse_known_args()

    here = Path(__file__).parent.resolve()
    build_path = Path(f"{here}/build")
    if parser_args.rebuild and build_path.exists():
        shutil.rmtree(build_path)

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
    
    raise Exception(f"zlib build not supported.")


if __name__ == "__main__":
    main()
