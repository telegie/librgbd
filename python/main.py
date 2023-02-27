import pyrgbd as rgbd
import cv2
import numpy as np
import requests
import os.path


def main():
    # Download file from server.
    base64url_video_id = "A9oofdweNJ4"
    video_file_path = f"tmp/{base64url_video_id}.mkv"
    if not os.path.exists(video_file_path):
        video_id = rgbd.decode_base64url_to_long(base64url_video_id)
        video_url = f"https://posts.telegie.com/v1/{video_id}/{video_id}.mkv"
        response = requests.get(video_url)
        with open(video_file_path, "wb") as file:
            file.write(response.content)

    # Extract information from the downloaded file.
    with rgbd.NativeFileParser(video_file_path) as native_file_parser:
        with native_file_parser.parse(True, True) as native_file:
            file = rgbd.File.from_native(native_file)

    # Decode color frames.
    color_arrays = []
    with rgbd.NativeColorDecoder(rgbd.ColorCodecType.VP8) as color_decoder:
        for video_frame in file.video_frames:
            yuv_frame = color_decoder.decode(video_frame.color_bytes)
            color_array = rgbd.convert_yuv420_to_rgb(
                yuv_frame.y_channel, yuv_frame.u_channel, yuv_frame.v_channel
            )
            color_arrays.append(color_array)

    # Decode depth frames.
    depth_frames = []
    with rgbd.NativeDepthDecoder(rgbd.DepthCodecType.TDC1) as depth_decoder:
        for video_frame in file.video_frames:
            depth_frame = depth_decoder.decode(video_frame.depth_bytes)
            depth_frames.append(depth_frame)

    cv2.imshow("color", cv2.cvtColor(color_arrays[0], cv2.COLOR_RGB2BGR))
    cv2.imshow("depth", depth_frames[0].values.astype(np.uint16))

    # for video_frame in file.video_frames:
    #     print(f"video timecode: {video_frame.global_timecode}")
    #
    # for imu_frame in file.imu_frames:
    #     print(f"timecode: {imu_frame.global_timecode}")
    #     print(f"gravity: {imu_frame.gravity}")

    # Render things.
    # points = []
    # colors = []
    # step = color_track.width / depth_track.width
    # for row in range(depth_track.height):
    #     for col in range(depth_track.width):
    #         direction = directions[row][col]
    #         depth = depth_array[row][col]
    #         points.append(direction * depth * 0.001)
    #
    #         color = color_arrays[0][int(row * step)][int(col * step)]
    #         # flip bgr to rgb
    #         color = np.array([color[0], color[1], color[2]])
    #         colors.append(color)
    #
    # points = np.array(points)
    # colors = np.array(colors)
    # points = vedo.Points(points, c=colors)
    # vedo.show(points)

    # directions = file.direction_table.to_np_array()
    # direction_xs = directions[:, :, 0].squeeze()
    # cv2.imshow("Direction X", np.absolute(direction_xs * 10000).astype(np.uint16))

    # direction_x_diffs = np.diff(direction_xs, axis=1)
    # # cv2.imshow("Direction X Diff", np.absolute(direction_x_diffs * 10000000).astype(np.uint16))
    # direction_x_diffs_delta = direction_x_diffs - np.mean(direction_x_diffs)
    # cv2.imshow(
    #     "Direction X Diff Delta",
    #     np.absolute(direction_x_diffs_delta * 1000000000).astype(np.uint16),
    # )

    # cv2.waitKey(0)


if __name__ == "__main__":
    main()
