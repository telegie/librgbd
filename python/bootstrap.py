import subprocess
import sys
from pathlib import Path


def main():
    here = Path(__file__).parent.resolve()
    librgbd_root = Path(__file__).parent.parent.resolve()
    subprocess.run(["python3", f"{librgbd_root}/build.py"] + sys.argv[1:], check=True)
    subprocess.run(["python3", f"{here}/_pyrgbd_cffi/_ffi_build.py"] + sys.argv[1:], check=True)


if __name__ == "__main__":
    main()
