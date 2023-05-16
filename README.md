# librgbd

[![MIT License](https://img.shields.io/github/license/telegie/librgbd)](https://en.wikipedia.org/wiki/MIT_License)
[![Build Status](https://github.com/telegie/librgbd/actions/workflows/build.yml/badge.svg)](https://github.com/telegie/librgbd/actions/workflows/build.yml)

librgbd is a C++ library for reading and writing RGBD video files based on libmatroska.

This library can read files exported from the Telegie.

## RGBD Video File Structure

The main functionality of librgbd is to read and write .mkv files that contain RGBD (color + depth) videos. RGBD videos are a 3D format in which each pixel has a color and depth value. This is the natural output of RGBD cameras, such as LiDAR and Kinect.

librgbd records video, audio, IMU, and camera calibration data into a single mkv file. 
<!-- Video data is recorded in two video Matroska video tracks (for Color and Depth respectively), audio in one audio track, and IMU in four separate subtitle tracks. -->

- Video data is stored in two tracks: one for color and one for depth. TDC1 stands for "Telegie Depth Codec 1" and is derived from a [prior work](https://github.com/hanseuljun/temporal-rvl). Please note that V_TDC1 is not a standard KaxCodecId included in [matroska.org](https://www.matroska.org/technical/codec_specs.html).
    | data | codec |
    |-|-|
    | color | VP8 (KaxCodecID: V_VP8) |
    | depth | TDC1 (KaxCodecID: V_TDC1) |

- Audio data is stored in one track and uses the Opus codec.

- IMU data is stored in four subtitle tracks:
    | data | units |
    |-|-|
    | acceleration | meters per second squared (m/s²) |
    | rotation rate | radians per second (rad/s) |
    | magnetic field | microtesla (µT) |
    | gravity | meters per second squared (m/s²) |

- Camera calibration data is stored in a json format as a Matroska attachment.

For more details, see [file_writer.cpp](src/file_writer.cpp) or [file_parser.cpp](src/file_parser.cpp).

# pyrgbd

pyrgbd is a Python binding for librgbd.

## PyPI packages
PyPI packages are available for Linux (x64) and macOS (x64/Apple Silicon) for CPython 3.6+. A package for Windows (x64) is not yet available, but is currently in testing. Stay tuned.

    pip install pyrgbd

## Manual Build/Installation

Linux Requirements

    add-apt-repository ppa:deadsnakes/ppa

    apt install python3.9-dev python3.9-distutils libgl1 python3-venv python3-cachecontrol

Setup

    git submodule update --init --recursive
    python3 bootstrap.py --rebuild
    pip install .

Run Example

    # From Root directory the project
    python3 example_python/pyrgbd_decoding_encoding/pyrgbd_example.py

    # See options
    python3 example_python/pyrgbd_decoding_encoding/pyrgbd_example.py -h
    
