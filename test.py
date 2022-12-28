#!/usr/bin/env python3
import platform
import subprocess
from pathlib import Path


def test_arm64_mac_binaries():
    here = Path(__file__).parent.resolve()
    subprocess.run(["ninja", "test"], cwd=f"{here}/build/arm64-mac", check=True)


def test_x64_mac_binaries():
    here = Path(__file__).parent.resolve()
    subprocess.run(["ninja", "test"], cwd=f"{here}/build/x64-mac", check=True)


def test_x64_linux_binaries():
    here = Path(__file__).parent.resolve()
    subprocess.run(["ninja", "test"], cwd=f"{here}/build/x64-linux", check=True)


def main():
    if platform.system() == "Windows":
        # TODO: implement
        return
    if platform.system() == "Darwin":
        if platform.machine() == "arm64":
            test_arm64_mac_binaries()
            # Arm64 Mac can run x64 Mac programs.
            test_x64_mac_binaries()
        elif platform.machine() == "x86_64":
            test_x64_mac_binaries()
        return
    elif platform.system() == "Linux":
        test_x64_linux_binaries()
        return

    raise Exception(f"librgbd test not supported.")


if __name__ == "__main__":
    main()
