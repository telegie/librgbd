rgbd video file format structure
----------------------------------

The main functionality of librgbd is to read and write .mkv files that contain RGBD (color + depth) videos. RGBD videos are a 3D format in which each pixel has a color and depth value. This is the natural output of RGBD cameras, such as LiDAR and Kinect.

librgbd records video, audio, IMU, and camera calibration data into a single mkv file. 
<!-- Video data is recorded in two video Matroska video tracks (for Color and Depth respectively), audio in one audio track, and IMU in four separate subtitle tracks. -->

- Video data is stored in two tracks: one for color and one for depth. TDC1 stands for "Telegie Depth Codec 1" and is derived from a [prior work](https://github.com/hanseuljun/temporal-rvl). Please note that V_TDC1 is not a standard KaxCodecId included in [matroska.org](https://www.matroska.org/technical/codec_specs.html).

.. list-table:: tracks
    :widths: 50 50
    :header-rows: 1

    * - data
      - codec
    * - color
      - VP8 (KaxCodecID: V_VP8)
    * - depth
      - TDC1 (KaxCodecID: V_TDC1)

- Audio data is stored in one track and uses the Opus codec.

- IMU data is stored in four subtitle tracks:

.. list-table:: tracks
    :widths: 50 50
    :header-rows: 1

    * - data
      - unit
    * - acceleration
      - meters per second squared (m/s²)
    * - rotation rate
      - radians per second (rad/s)
    * - magnetic field
      - microtesla (µT)
    * - gravity
      - meters per second squared (m/s²)

- Camera calibration data is stored in a json format as a Matroska attachment.

For more details, see (src/file_writer.cpp) or (src/file_parser.cpp) in librgbd's repository.