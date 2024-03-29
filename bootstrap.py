#!/usr/bin/env python3
import platform
import subprocess
import sys
from pathlib import Path


def main():
    here = Path(__file__).parent.resolve()
    subprocess.run(["python3", f"{here}/deps/libpng-build/build.py"] + sys.argv[1:], check=True)


if __name__ == "__main__":
    main()
