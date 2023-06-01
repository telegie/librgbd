import pyrgbd as rgbd
import argparse
import examples_utils
import numpy as np
import cv2


def main():
    parser = argparse.ArgumentParser(
        prog="pyrgbd example imshow",
        description="pyrgbd example of showing video frames using OpenCV.",
    )
    parser.add_argument("-i", "--input", help="path to input 3D video file")

    args = parser.parse_args()
    if args.input is None:
        print(
            "No input file path specified. You can download a sample video from https://telegie.com/posts/A9oofdweNJ4"
        )
        return

    # Parse the video file (record).
    record_parser = rgbd.RecordParser(args.input)
    record = record_parser.parse(True, True)

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

    # Write color video frames into .jpg files.
    for index in range(len(record_video_frames)):
        yuv_frame = yuv_frames[index]
        rgb_array = examples_utils.convert_yuv_frame_to_rgb_array(yuv_frame)

        depth_frame = depth_frames[index]
        depth_array = depth_frame.get_values().astype(np.uint16)

        # Improve visibility when shown via cv2.imshow.
        rgb_array = cv2.cvtColor(rgb_array, cv2.COLOR_BGR2RGB)
        depth_array = depth_array * 20

        cv2.imshow("color", rgb_array)
        cv2.imshow("depth", depth_array)
        cv2.waitKey(1)


if __name__ == "__main__":
    main()
