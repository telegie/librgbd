import io
from pathlib import Path
from setuptools import setup, find_packages


here = Path(__file__).resolve().parent
long_description = io.open(f"{here}/README.md", encoding="utf-8").read()

# TODO: Make this work with other platforms and other python versions.
PYRGBD_PACKAGE_DATA = [
    f"{here}/pyrgbd/_librgbd_ffi.cpython-310-darwin.so",
    f"{here}/pyrgbd/_librgbd/arm64-mac/bin/librgbd.dylib"
]

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
