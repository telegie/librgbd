import argparse
import pyrgbd as rgbd
import cv2
import examples_utils
import numpy as np


def main():
    parser = argparse.ArgumentParser(
        prog="pyrgbd example import_record3d",
        description="pyrgbd example of importing RGBD videos recorded by Record3D.",
    )
    parser.add_argument("-i", "--input", help="Path to input .mp4 file exported from Record3D. Export depth in hue on the left-side.")
    parser.add_argument("-o", "--output", help="Path to output video file.")

    args = parser.parse_args()
    if args.input is None:
        print(
            "No input file path specified."
        )
        return
    if args.output is None:
        print(
            "No output video path specified."
        )
        return

    cap = cv2.VideoCapture(args.input)
    rgb_arrays = []
    depth_arrays = []
    while cap.isOpened():
        ret, frame = cap.read()
        # if frame is read correctly ret is True
        if not ret:
            print("Can't receive frame (stream end?). Exiting ...")
            break
        # right-side is color
        bgr_array = frame[:,(frame.shape[1] // 2):,:]
        # hue of left-side is depth
        depth_in_hue_bgr_array = frame[:,:(frame.shape[1] // 2),:]
        depth_in_hue_hsv_array = cv2.cvtColor(depth_in_hue_bgr_array, cv2.COLOR_BGR2HSV)
        depth_array = depth_in_hue_hsv_array[:,:,0]
        cv2.imshow('color', bgr_array)
        cv2.imshow('depth', depth_array)
        if cv2.waitKey(1) == ord('q'):
            break

        rgb_array = cv2.cvtColor(bgr_array, cv2.COLOR_BGR2RGB)

        # The conversion formula is found from below:
        # https://github.com/marek-simonik/record3d-wifi-streaming-and-rgbd-mp4-3d-video-demo/blob/master/js/app/pointcloud-material.js
        depth_array = depth_array.astype(float)
        depth_array = depth_array / 255.0
        depth_array = depth_array * 3.0
        # Convert meters to millimeters as unit is meters for Record3D
        # but millimeters by default in librgbd.
        depth_array = depth_array * 1000.0
        depth_array = depth_array.astype(np.int32)

        rgb_arrays.append(rgb_array)
        depth_arrays.append(depth_array)
    cap.release()

    focal_length = 525
    width = rgb_arrays[0].shape[1]
    height = rgb_arrays[0].shape[0]

    record3d_calibration = rgbd.UndistortedCameraCalibration(
        width, height, width, height, focal_length / width, focal_length / height, 0.5, 0.5
    )
    record_builder = rgbd.RecordBuilder()
    record_builder.set_calibration(record3d_calibration)

    color_encoder = rgbd.ColorEncoder(rgbd.ColorCodecType.VP8, width, height)
    depth_encoder = rgbd.DepthEncoder(rgbd.DepthCodecType.TDC1, width, height)
    for index in range(len(rgb_arrays)):
        rgb_array = rgb_arrays[index]
        depth_array = depth_arrays[index]
        if index % 100 == 0:
            print(f"encoding frame {index}")
        time_point_us = index * 1000 * 1000 // 30
        keyframe = index % 60 == 0

        yuv_frame = examples_utils.convert_rgb_array_to_yuv_frame(rgb_array)

        color_bytes = color_encoder.encode(yuv_frame, keyframe)
        depth_bytes = depth_encoder.encode(depth_array, keyframe)

        record_video_frame = rgbd.RecordVideoFrame(
            time_point_us, keyframe, color_bytes, depth_bytes
        )
        record_builder.add_video_frame(record_video_frame)

    record_builder.build_to_path(args.output)


if __name__ == "__main__":
    main()
