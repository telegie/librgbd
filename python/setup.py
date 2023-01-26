import io
import platform
from pathlib import Path
from setuptools import setup, find_packages


here = Path(__file__).resolve().parent
long_description = io.open(f"{here}/README.md", encoding="utf-8").read()

# TODO: Improve how this handles python versions.
if platform.system() == "Windows":
    # TODO: Add Windows ffi so files.
    PYRGBD_PACKAGE_DATA = [
        f"{here}/pyrgbd/librgbd.dll",
        f"{here}/pyrgbd/libwinpthread-1.dll",
        f"{here}/pyrgbd/zlib1.dll",
        f"{here}/pyrgbd/avcodec-58.dll",
        f"{here}/pyrgbd/avutil-56.dll",
    ]
elif platform.system() == "Darwin":
    PYRGBD_PACKAGE_DATA = [
        f"{here}/pyrgbd/librgbd.dylib",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-36-darwin.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-37-darwin.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-38-darwin.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-39-darwin.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-310-darwin.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-311-darwin.so",
    ]
elif platform.system() == "Linux":
    PYRGBD_PACKAGE_DATA = [
        f"{here}/pyrgbd/librgbd.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-36-x86_64-linux-gnu.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-37-x86_64-linux-gnu.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-38-x86_64-linux-gnu.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-39-x86_64-linux-gnu.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-310-x86_64-linux-gnu.so",
        f"{here}/pyrgbd/_librgbd_ffi.cpython-311-x86_64-linux-gnu.so",
    ]
else:
    raise Exception(f"Unknown platform.system(): {platform.system()}")

setup(
    name="pyrgbd",
    version="0.1.0",
    description="Library for reading and writing RGBD videos.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/telegie/pyrgbd",
    author="Hanseul Jun",
    author_email="hanseul@telegie.com",
    license="MIT",
    packages=find_packages(),
    install_requires=["cffi", "numpy", "opencv-python", "pyglm"],
    setup_requires=["cffi"],
    # For files to be copied by package_data, they should exist inside the corresponding package's directory.
    package_data={
        "pyrgbd": PYRGBD_PACKAGE_DATA
    },
    cffi_modules=[
        "./pyrgbd/_build_librgbd_ffi.py:ffi"
    ]
)
