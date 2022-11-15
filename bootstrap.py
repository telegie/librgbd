#!/usr/bin/env python3
import subprocess
from pathlib import Path


def main():
    here = Path(__file__).parent.resolve()
    subprocess.run(["python3", f"{here}/deps/ffmpeg-binaries/build.py"])
    subprocess.run(["python3", f"{here}/deps/libpng-binaries/build.py"])


if __name__ == "__main__":
    main()
