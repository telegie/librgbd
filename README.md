# librgbd

librgbd is a C++/Python library for reading and writing RGBD video files and is based on libmatroska of the .mkv files.

[Docs](https://telegie.github.io/librgbd) | [Viewer](https://telegie.com/file-player)

[![MIT License](https://img.shields.io/github/license/telegie/librgbd)](https://en.wikipedia.org/wiki/MIT_License)
[![Build Status](https://github.com/telegie/librgbd/actions/workflows/build.yml/badge.svg)](https://github.com/telegie/librgbd/actions/workflows/build.yml)

Core features of librgbd include:
- Compression of color using VP8.
- Compression of depth using a depth video codec.  
- Storage and access of IMU information and camera pose in a well-defined and documented coordinate system.
- Storage and access of per-frame camera calibration information (i.e., ARKit calibration support).
- Mapping/unmapping of depth pixels to/from 3D space with lens distortion taken into account.
- Support of multiple camera parameter sets: Azure Kinect, iOS (AVFoundation and ARKit), and pinhole.
- Undistortion of videos with lens distortion.
- Compatibility with 2D video players. For example, VLC player can play the color and audio tracks of our .mkv files.

While the easiest way to capture our .mkv files is using Telegie (https://apple.co/3qaycil), librgbd aims and is designed to support RGBD videos recorded from any RGBD camera or application.

## Usage

### Python via Pypi (Mac and Linux)
- pip install pyrgbd

### C++ via Source
- Run boostrap.py
- Add to your project via CMake add_subdirectory.

### Python via Source
- python boostrap.py
- pip install -e .
