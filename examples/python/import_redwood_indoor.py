from os import listdir
from os.path import isfile, join
import argparse
import pyrgbd as rgbd
import cv2
import examples_utils
import numpy as np


def main():
    parser = argparse.ArgumentParser(
        prog="pyrgbd example import_redwood_indoor",
        description="pyrgbd example of importing RGBD videos from http://redwood-data.org/indoor_lidar_rgbd.",
    )
    parser.add_argument("-i", "--input", help="Path to input dataset.")
    parser.add_argument("-o", "--output", help="Path to output 3D video file.")
    parser.add_argument("-f", "--frames", type=int, help="Number of frames to save in the output file.")

    args = parser.parse_args()
    if args.input is None:
        print(
            "No input file path specified. You can download a sample video from https://telegie.com/posts/DTosAKtQuvc"
        )
        return
    if args.output is None:
        print(
            "No output folder path specified."
        )
        return
    print(f"frames: {args.frames}")

    image_folder_path = f"{args.input}/image"
    image_filenames = [f for f in listdir(image_folder_path) if isfile(join(image_folder_path, f))]
    image_filenames.sort()
    for image_filename in image_filenames:
        print(f"image_filename: {image_filename}")

    depth_folder_path = f"{args.input}/depth"
    depth_filenames = [f for f in listdir(depth_folder_path) if isfile(join(depth_folder_path, f))]
    depth_filenames.sort()
    for depth_filename in depth_filenames:
        print(f"depth_filename: {depth_filename}")

    focal_length = 525
    width = 640
    height = 480

    redwood_indoor_calibration = rgbd.UndistortedCameraCalibration(
        width, height, width, height, focal_length / width, focal_length / height, 0.5, 0.5
    )
    record_builder = rgbd.RecordBuilder()
    record_builder.set_calibration(redwood_indoor_calibration)

    color_encoder = rgbd.ColorEncoder(rgbd.ColorCodecType.VP8, width, height)
    depth_encoder = rgbd.DepthEncoder(rgbd.DepthCodecType.TDC1, width, height)
    for index in range(len(image_filenames)):
        if index % 100 == 0:
            print(f"encoded frame {index}")
        if index == args.frames:
            break
        time_point_us = index * 1000 * 1000 // 30
        keyframe = index % 60 == 0
        image_filename = image_filenames[index]
        depth_filename = depth_filenames[index]
        image_file_path = f"{image_folder_path}/{image_filename}"
        depth_file_path = f"{depth_folder_path}/{depth_filename}"

        rgb_array = cv2.imread(image_file_path)
        depth_array = cv2.imread(depth_file_path, cv2.IMREAD_ANYDEPTH)
        cv2.imshow("color", rgb_array)
        cv2.imshow("depth", depth_array)
        cv2.waitKey(1)

        yuv_frame = examples_utils.convert_rgb_array_to_yuv_frame(rgb_array)
        depth_array = depth_array.astype(np.int32)

        color_bytes = color_encoder.encode(yuv_frame, keyframe)
        depth_bytes = depth_encoder.encode(depth_array, keyframe)

        record_video_frame = rgbd.RecordVideoFrame(
            time_point_us, keyframe, color_bytes, depth_bytes
        )
        record_builder.add_video_frame(record_video_frame)

    record_builder.build_to_path(args.output)


if __name__ == "__main__":
    main()
