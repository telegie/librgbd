import io
from pathlib import Path
from setuptools import setup, find_packages


here = Path(__file__).resolve().parent
long_description = io.open(f"{here}/README.md", encoding="utf-8").read()


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
    # packages=["pyrgbd"],
    packages=find_packages(),
    # install_requires=["cffi", "numpy", "opencv-python", "pyglm"],
    install_requires=["cffi"],
    setup_requires=["cffi"],
    # For files to be copied by package_data, they should exist inside the corresponding package's directory.
    # package_data={
    #     "pyrgbd": ["_librgbd.cpython-39-darwin.so",
    #                "_librgbd.cpython-310-darwin.so",
    #                f"{librgbd_root}/output/arm64-mac/bin/librgbd.dylib",
    #                f"{librgbd_root}/output/x64-mac/bin/librgbd.dylib",
    #                "_librgbd.cpython-39-x86_64-linux-gnu.so",
    #                "_librgbd.cpython-310-x86_64-linux-gnu.so",
    #                f"{librgbd_root}/output/x64-linux/bin/librgbd.so"]
    # },
    package_data={
        "pyrgbd": [
            f"{here}/pyrgbd/_librgbd/arm64-mac/bin/librgbd.dylib",
            f"{here}/pyrgbd/_librgbd/arm64-mac/include/rgbd/rgbd_capi.h",
        ]
    },
    cffi_modules=[
        "./pyrgbd/_librgbd_ffi.py:ffi"
    ]
)
