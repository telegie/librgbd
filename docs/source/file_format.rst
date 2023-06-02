File Format
==================================
A main functionality of librgbd is to read and write a 3D video file format based on the `Matroska Media Container <https://www.matroska.org/>`_ of the .mkv files.
Our 3D video file format includes followings as Matroska tracks:

- Color Frame
- Depth Frame
- Audio Frame
- IMU Data
- Camera Pose
- Per-Frame Camera Calibration Parameters

Followings are included as Matroska attachments:

- Camera Calibration Parameters
- Cover Image

Color, Depth, and Audio Frames
----------------------------------
For color, depth, and audio frames, we use VP8, TDC1, and Opus as their codecs.
TDC1 stands for "Telegie Depth Codec 1" and is derived from `Temporal RVL <https://github.com/hanseuljun/temporal-rvl>`_.
Note that V_TDC1 is not a standard KaxCodecId included in `Matroska's codec specs <https://www.matroska.org/technical/codec_specs.html>`_.

.. list-table::
    :widths: 50 50
    :header-rows: 1

    * - type
      - codec
    * - color
      - VP8 (KaxCodecID: V_VP8)
    * - depth
      - TDC1 (KaxCodecID: V_TDC1)
    * - audio
      - Opus (KaxCodecID: A_OPUS)

IMU Data
----------------------------------
IMU data is stored in four subtitle tracks.
As not all 3D video files are from a device, or since the recording device may not provide IMU data, IMU data may not exist in a 3D video file.
Gravity information is often provided via sensor fusion implemented by the manufacturer of the recording device.
We save this information as it is based on IMU data with higher frequency than what we save in 3D video files, thus not possible to replicate the quality during post-processing.

.. list-table::
    :widths: 50 50
    :header-rows: 1

    * - type
      - unit
    * - acceleration
      - meters per second squared (m/s²)
    * - rotation rate
      - radians per second (rad/s)
    * - magnetic field
      - microtesla (µT)
    * - gravity
      - meters per second squared (m/s²)

Camera Pose
----------------------------------
Camera poses are stored in two subtitle tracks.
One track contains one 3D vector per frame and represents camera translation.
One track contains one quaternion per frame and represents camera rotation.
Camera data may not exist in a 3D video file and is often added during the post-processing step after recording.

Camera Calibration Parameters
----------------------------------
Camera Calibration Parameters can be found from two places.
Every 3D video file must include one set of parameters as a Matroska attachment.
Optionally, a 3D video file may include a Matroska track for calibration frames.
When this track exists, the calibration parameters from this track should override the parameters from the attachment.
Frame-level calibration support is mainly for support of AR frameworks.
AR frameworks often expose their pre-processed video frames to the app layer together with a calibration parameter set that contain minor difference between each frame.

Cover Image
----------------------------------
3D video files can include a cover image to help file explorers.
Currently, 600x600 .png files are being used.
For more information, see `Matroska's documentation <https://www.matroska.org/technical/attachments.html>`_.
