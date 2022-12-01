#!/usr/bin/env python3
import subprocess
from pathlib import Path


def main():
    here = Path(__file__).parent.resolve()
    subprocess.run(["python3", f"{here}/deps/ffmpeg-binaries/build.py"], check=True)
    subprocess.run(["python3", f"{here}/deps/libpng-binaries/build.py"], check=True)


if __name__ == "__main__":
    main()
