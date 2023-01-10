import io
from pathlib import Path
from setuptools import setup


# This part is to create bdist_wheel to let setuptools know that this package is not pure python.
# https://stackoverflow.com/questions/45150304/how-to-force-a-python-wheel-to-be-platform-specific-when-building-it
try:
    from wheel.bdist_wheel import bdist_wheel as _bdist_wheel
    class bdist_wheel(_bdist_wheel):
        def finalize_options(self):
            _bdist_wheel.finalize_options(self)
            self.root_is_pure = False
except ImportError:
    bdist_wheel = None

here = Path(__file__).parent.resolve()
root = Path(__file__).parent.parent.resolve()
long_description = io.open(f"{here}/README.md", encoding="utf-8").read()

setup(
    name="pyrgbd",
    version="0.1.0",
    description="Library for reading and writing RGBD videos",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/telegie/pyrgbd",
    license="MIT",
    packages=['pyrgbd'],
    install_requires=["cffi", "numpy", "opencv-python", "pyglm"],
    setup_requires=["cffi"],
    # For files to be copied by package_data, they should exist inside the corresponding package's directory.
    package_data={
        'pyrgbd': ["_librgbd.cpython-39-darwin.so",
                   "_librgbd.cpython-310-darwin.so",
                   f"{root}/output/arm64-mac/bin/librgbd.dylib",
                   f"{root}/output/x64-mac/bin/librgbd.dylib",
                   "_librgbd.cpython-39-x86_64-linux-gnu.so",
                   "_librgbd.cpython-310-x86_64-linux-gnu.so",
                   f"{root}/output/x64-linux/bin/librgbd.so"]
    },
    cmdclass={'bdist_wheel': bdist_wheel}
)
