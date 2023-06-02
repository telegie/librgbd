Tutorial
==================================
We assume you have already installed pyrgbd and will run examples in the examples/python directory of the librgbd git repository.
If you have installed pyrgbd via PyPI, go to https://github.com/telegie/librgbd/tree/main/examples/python and download the files.

Install Dependencies for Examples
----------------------------------
There are additional dependencies to run the example scripts.
The list of depedencies are in examples/python/requirements.txt and can be installed as following.

.. code-block::

    pip install -r requirements.txt

Download a 3D Video
----------------------------------
You can obtain a 3D video file by downloading one from https://telegie.com/posts.
You can skip this step if you already have a 3D video file.
Run download.py in the examples/python directory as following.

.. code-block::

    python download.py -i https://telegie.com/posts/DTosAKtQuvc -o cat.mkv

Now you will see the 3D video downloaded as cat.mkv.
We will use this file for the following steps of this tutorial.

Connect Color and Depth Frames to OpenCV
----------------------------------------
In imshow.py, the color and depth frames of a 3D video file gets decoded and gets rendered via OpenCV.  

.. code-block::

    python imshow.py -i cat.mkv

Save Color and Depth Frames into JPEG and PNG Files
---------------------------------------------------
In decode.py, the color and depth frames of a 3D video file gets decoded and gets saved into .jpg and .png files named after the frame indices.
This step similar to this example is often required to use existing software that take JPG and PNG files as their input.

.. code-block::

    python decode.py -i cat.mkv -o cat

Import Redwood Indoors Dataset
------------------------------
In import_redwood_indoor.py, the color and depth frames in the form of .jpg and .png files get encoded into a 3D video file.
After downloading their `Apartment <https://drive.google.com/file/d/1TqoIE1Q1M30q8FBQ_dMcZj9JR6G0ztI5>`_ data and unzipping it,
you can run below and convert the Apartment data files into apartment.mkv.
This step similar to this example is often required convert existing datasets into 3D video files.
The -f flag is the number of frames to encode to the 3D video file and 300 frames corresponds to the first 10 seconds.
This is set here in this example only to enable following this tutorial quicker as the whole Apartment data is 31919 frames long.
You can remove this flag to encode the whole data.

.. code-block::

    python import_redwood_indoor.py -i {path/to/the/apartment/folder} -o apartment.mkv -f 300

Note that the following lines of import_redwood_indoor.py is setting the camera calibration parameters.
These calibration parameters were posted on the dataset website and were not obtainable from the dataset files.

.. code-block::

    focal_length = 525
    width = 640
    height = 480

    redwood_indoor_calibration = rgbd.UndistortedCameraCalibration(
        width, height, width, height, focal_length / width, focal_length / height, 0.5, 0.5
    )
    record_builder = rgbd.RecordBuilder()
    record_builder.set_calibration(redwood_indoor_calibration)

Watch a 3D Video
------------------------------
You can watch a 3D video by using a web player at https://telegie.com/file-player.
