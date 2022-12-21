#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path


def main():
    here = Path(__file__).parent.resolve()
    args = ["python3", f"{here}/deps/libpng-binaries/build.py"]
    args += sys.argv[1:]
    subprocess.run(args, check=True)


if __name__ == "__main__":
    main()
