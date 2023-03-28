# librgbd

[![MIT License](https://img.shields.io/github/license/telegie/librgbd)](https://en.wikipedia.org/wiki/MIT_License)
[![Build Status](https://github.com/telegie/librgbd/actions/workflows/build.yml/badge.svg)](https://github.com/telegie/librgbd/actions/workflows/build.yml)

librgbd is a C++ library for read and writing RGBD videos files based on libmatroska.

This library can read files exported from the Telegie.

## RGBD Video File Structure

The main functionality of librgbd is to read and write .mkv files holding RGBD (color + depth) videos. RGBD videos are a 3D format that each pixel has a color and depth value, which is the natural outcome from RGBD cameras (e.g., LiDAR and Kinect).

librgbd records video, audio, IMU, and camera calibration data. Video data is recorded in 2 video Matroska video tracks, audio in 1 audio track, and IMU in 4 subtitle tracks.

Video data is in color and depth tracks. The color track is in VP8 codec (KaxCodecID: V_VP8) and depth track is in TDC1 codec (KaxCodecID: V_TDC1). V_TDC1 is a non-standard KaxCodecID (not included in [matroska.org](https://www.matroska.org/technical/codec_specs.html)). It stands for Telegie Depth Codec 1 and is derived from [prior work](https://github.com/hanseuljun/temporal-rvl).

Audio data is in one track. Opus codec is used.

IMU data are in acceleration, rotation rate, magnetic field, and gravity tracks. The units are in m/s^2, radian/sec, uT (micro Tesla), and m/s^2.

Camera calibration data are saved in a json format as a Matroska attachment.

For more details, see [file_writer.cpp](src/file_writer.cpp) or [file_parser.cpp](src/file_parser.cpp).

## pyrgbd pybind11

Build: CMAKE_ARGS="-D CMAKE_OSX_ARCHITECTURES=arm64" python setup.py install

Faster linux build: python setup.py install -j8

Test: python test_pyrgbd.py

# pyrgbd

pyrgbd is a Python library for read and writing RGBD videos files based on libmatroska.

This library can read files exported from the Telegie and is based on librgbd.

## Linux Requirements

- add-apt-repository ppa:deadsnakes/ppa
- apt install python3.9-dev python3.9-distutils libgl1 python3-venv python3-cachecontrol

## Setup
- git submodule update --init --recursive
- python3 bootstrap.py
- poetry shell
- poetry install
- python setup.py install

## Check
- mypy .
- black .

## Deploy
- python -m build --wheel
- twine check dist/*
- twine upload dist/*
