import pyrgbd as rgbd
import argparse
import examples_utils
from PIL import Image
import numpy as np
import imageio
import os


def main():
    parser = argparse.ArgumentParser(
        prog="pyrgbd example decode",
        description="pyrgbd example of decoding a 3D video file.",
    )
    parser.add_argument("-i", "--input", help="Path to input 3D video file")
    parser.add_argument("-o", "--output", help="Path to output directory")

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

    # Parse the video file (record).
    record_parser = rgbd.RecordParser(args.input)
    record = record_parser.parse(True)

    # Print basic codec info.
    record_tracks = record.get_tracks()
    print(f"color width: {record_tracks.color_track.width}")
    print(f"color height: {record_tracks.color_track.height}")
    print(f"color codec: {record_tracks.color_track.codec}")
    print(f"depth width: {record_tracks.depth_track.width}")
    print(f"depth height: {record_tracks.depth_track.height}")
    print(f"depth codec: {record_tracks.depth_track.codec}")

    # Obtain encoded video frames.
    record_video_frames = record.get_video_frames()

    # Decode color video frames.
    yuv_frames = []
    color_decoder = rgbd.ColorDecoder(record_tracks.color_track.codec)
    for video_frame in record_video_frames:
        yuv_frames.append(color_decoder.decode(video_frame.get_color_bytes()))

    # Decode depth video frames.
    depth_frames = []
    depth_decoder = rgbd.DepthDecoder(record_tracks.depth_track.codec)
    for video_frame in record_video_frames:
        depth_frames.append(depth_decoder.decode(video_frame.get_depth_bytes()))

    if not os.path.exists(args.output):
        os.mkdir(args.output)
    # Write color video frames into .jpg and .png files.
    for index in range(len(yuv_frames)):
        yuv_frame = yuv_frames[index]
        rgb_array = examples_utils.convert_yuv_frame_to_rgb_array(yuv_frame)

        depth_frame = depth_frames[index]
        depth_array = depth_frame.get_values().astype(np.uint16)

        # Write color frame into a .jpg file.
        color_image = Image.fromarray(rgb_array)
        color_image.save(f"{args.output}/color_{index:03d}.jpg")

        # Write depth frame into a .png file.
        imageio.imwrite(f"{args.output}/depth_{index:03d}.png", depth_array)


if __name__ == "__main__":
    main()
