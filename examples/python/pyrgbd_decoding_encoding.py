import pyrgbd as rgbd
import argparse
import glm
import base64
import pickle
from . import examples_utils


def parse_arguments():
    parser = argparse.ArgumentParser(
        prog="python pyrgbd_example",
        description="pyrgbd example for decoding and encoding video files with depth info and posts from telegie.com.",
    )
    parser.add_argument("-f", "--file", help="video file with depth")
    parser.add_argument(
        "-u",
        "--url",
        help="telegie.com post url",
        default="https://telegie.com/posts/DTosAKtQuvc",
    )
    args = parser.parse_args()
    if (
        args.file is None
        and args.url
        == "parser.add_argument('-f', '--file', help='video file with depth')"
    ):
        print(
            "No file or url specified. Downloading a default sample video from https://telegie.com/posts/DTosAKtQuvc"
        )
    return args


def get_video_file_path(args):
    # Download a file from telegie.com post if no file is specified.
    video_file_path = args.file
    if video_file_path is None and args.url:
        video_file_path = f"videos/temp.mkv"
        if not acquire_rgbd_video_file_from_telegie(video_file_path, args.url):
            print("cannot download from telegie.com")
            return ""
    return video_file_path


def decode_base64url_to_long(s: str):
    return int.from_bytes(base64.urlsafe_b64decode(s + "==="), "big")


def acquire_rgbd_video_file_from_telegie(video_file_path: str, telegie_post_url: str):
    try:
        import requests

        if telegie_post_url.endswith("\\"):
            telegie_post_url = telegie_post_url[0:-1]
        base64url_video_id = telegie_post_url.split("/")[-1]
        video_id = decode_base64url_to_long(base64url_video_id)
        telegie_video_url = (
            f"https://posts.telegie.com/v1/{video_id}/{video_id}.mkv"
        )

        response = requests.get(telegie_video_url)
        with open(video_file_path, "wb+") as record:
            record.write(response.content)
        return True
    except:
        return False


def parse_video(video_record_path):
    record_parser = rgbd.RecordParser(video_record_path)
    return record_parser.parse(True, True)


def get_builder(depth_unit):
    standard_calibration = rgbd.UndistortedCameraCalibration(
        1024, 1024, 512, 512, 0.5, 0.5, 0.5, 0.5
    )
    record_bytes_builder = rgbd.RecordBytesBuilder()
    record_bytes_builder.set_calibration(standard_calibration)
    record_bytes_builder.set_depth_unit(depth_unit)
    return standard_calibration, record_bytes_builder


def get_yuv_frames(codec, frame_mapper, record_video_frames):
    yuv_frames = []
    color_decoder = rgbd.ColorDecoder(codec)
    for video_frame in record_video_frames:
        yuv_frame = color_decoder.decode(video_frame.get_color_bytes())
        mapped_color_frame = frame_mapper.map_color_frame(yuv_frame)
        yuv_frames.append(mapped_color_frame)
        pickle.dumps(yuv_frame)
    return yuv_frames


def get_depth_frames(codec, frame_mapper, record_video_frames):
    depth_frames = []
    depth_decoder = rgbd.DepthDecoder(codec)
    for video_frame in record_video_frames:
        depth_frame = depth_decoder.decode(video_frame.get_depth_bytes())
        mapped_depth_frame = frame_mapper.map_depth_frame(depth_frame)
        depth_frames.append(mapped_depth_frame)
    return depth_frames


def get_time_point_us_for_frames(record_video_frames):
    time_point_us_for_frames = []
    for video_frame in record_video_frames:
        time_point_us_for_frames.append(video_frame.time_point_us)
    return time_point_us_for_frames


def get_frame_dimensions(color_frames, depth_frames):
    return (
        color_frames[0].width,
        color_frames[0].height,
        depth_frames[0].width,
        depth_frames[0].height,
    )


def set_video_depth_frames(
    record_bytes_builder, color_frames, depth_frames, time_point_us_for_frames
):
    # Set dimensions from the first frame
    color_width, color_height, depth_width, depth_height = get_frame_dimensions(
        color_frames, depth_frames
    )
    print(f"color_width: {color_width}, color_height: {color_height}")
    print(f"depth_width: {depth_width}, depth_height: {depth_height}")

    color_encoder = rgbd.ColorEncoder(
        rgbd.ColorCodecType.VP8, color_width, color_height
    )
    depth_encoder = rgbd.DepthEncoder(
        rgbd.DepthCodecType.TDC1, depth_width, depth_height
    )

    num_frames = len(color_frames)
    for index in range(num_frames):
        print(f"Video/Depth Frame: {index + 1} / {num_frames}")
        # keyframe for every 2 seconds
        is_keyframe = index % (rgbd.VIDEO_FRAME_RATE * 2) == 0

        color_frame = color_frames[index]
        depth_frame = depth_frames[index]
        color_bytes = color_encoder.encode(color_frame, is_keyframe)
        depth_bytes = depth_encoder.encode(depth_frame.get_values(), is_keyframe)
        time_point_us = time_point_us_for_frames[index]

        record_bytes_builder.add_video_frame(
            rgbd.RecordVideoFrame(time_point_us, is_keyframe, color_bytes, depth_bytes)
        )


def set_audio_frames(record_bytes_builder, audio_frames):
    num_frames = len(audio_frames)
    for index in range(num_frames):
        print(f"Audio Frame: {index + 1} / {num_frames}")
        audio_frame = audio_frames[index]
        record_bytes_builder.add_audio_frame(audio_frame)


def set_pose_frames_from_imu_frames(record_bytes_builder, imu_frames):
    # Set starting values
    previous_rotation = glm.quat(1, 0, 0, 0)
    previous_time_point_us = 0
    num_frames = len(imu_frames)
    if num_frames > 0:
        previous_time_point_us = imu_frames[0].time_point_us

    # Add frames
    for index in range(num_frames):
        print(f"IMU Frame: {index + 1} / {num_frames}")
        imu_frame = imu_frames[index]
        record_bytes_builder.add_imu_frame(imu_frame)

        delta_time_sec = (
            (imu_frame.time_point_us - previous_time_point_us) / 1000.0 / 1000.0
        )
        rotation = rgbd.MathUtils.apply_rotation_rate_and_gravity_to_rotation(
            previous_rotation,
            delta_time_sec,
            imu_frame.rotation_rate,
            imu_frame.gravity,
        )

        record_bytes_builder.add_pose_frame(
            rgbd.RecordPoseFrame(imu_frame.time_point_us, glm.vec3(0, 0, 0), rotation)
        )
        previous_rotation = rotation
        previous_time_point_us = imu_frame.time_point_us


def main():
    args = parse_arguments()

    # Use the file specified in args or download telegie.com post
    video_record_path = get_video_file_path(args)

    # Parse the video record(file)
    record = parse_video(video_record_path)

    # Print basic codec info for
    record_tracks = record.get_tracks()
    print(f"color codec: {record_tracks.color_track.codec}")
    print(f"depth codec: {record_tracks.depth_track.codec}")

    # get record bytes builder using the video
    standard_calibration, record_bytes_builder = get_builder(
        record_tracks.depth_track.depth_unit
    )

    # Set frame mappers and get video frames
    record_attachments = record.get_attachments()
    frame_mapper = rgbd.FrameMapper(
        record_attachments.camera_calibration, standard_calibration
    )
    record_video_frames = record.get_video_frames()

    ## Set cover (album art/thumbnail)
    # record_bytes_builder.set_cover_png_bytes(
    #     color_frames[0].get_mkv_cover_sized().get_png_bytes()
    # )
    # record_bytes_builder.set_cover_png_bytes(record_attachments.cover_png_bytes)
    record_bytes_builder.set_cover_png_bytes(None)

    # Build color/depth/audio/imu frames from existing frames (color frames are in yuv format)
    color_frames = get_yuv_frames(
        record_tracks.color_track.codec, frame_mapper, record_video_frames
    )
    depth_frames = get_depth_frames(
        record_tracks.depth_track.codec, frame_mapper, record_video_frames
    )
    time_point_us_for_frames = get_time_point_us_for_frames(record_video_frames)
    audio_frames = record.get_audio_frames()
    imu_frames = record.get_imu_frames()

    # Add video/depth frames into builder
    set_video_depth_frames(
        record_bytes_builder, color_frames, depth_frames, time_point_us_for_frames
    )

    # Add audio frames into builder
    set_audio_frames(record_bytes_builder, audio_frames)

    # Add imu (inertia measure unit) frames into builder
    set_pose_frames_from_imu_frames(record_bytes_builder, imu_frames)

    # Save file
    record_bytes_builder.build_to_path(video_record_path + "-reprocessed.mkv")


if __name__ == "__main__":
    main()
