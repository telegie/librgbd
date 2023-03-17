import pyrgbd as rgbd
import requests
import os.path
import glm
import base64


def decode_base64url_to_long(s: str):
    return int.from_bytes(base64.urlsafe_b64decode(s + "==="), "big")


def main():
    video_track = rgbd.FileColorVideoTrack()
    video_track.width = 10
    print(f"video_track.width: {video_track.width}")

    base64url_video_id = "A9oofdweNJ4"
    video_file_path = f"videos/{base64url_video_id}.mkv"
    # video_file_path = f"videos/Office.mkv"
    if not os.path.exists(video_file_path):
        video_id = decode_base64url_to_long(base64url_video_id)
        video_url = f"https://posts.telegie.com/v1/{video_id}/{video_id}.mkv"
        response = requests.get(video_url)
        with open(video_file_path, "wb+") as file:
            file.write(response.content)

    file_parser = rgbd.FileParser(video_file_path)
    file = file_parser.parse(True, True)

    print(f"color codec: {file.tracks.color_track.codec}")
    print(f"depth codec: {file.tracks.depth_track.codec}")

    standard_calibration = rgbd.UndistortedCameraCalibration(
        1024, 1024, 512, 512, 0.5, 0.5, 0.5, 0.5
    )

    file_bytes_builder = rgbd.FileBytesBuilder()
    file_bytes_builder.set_calibration(standard_calibration)
    file_bytes_builder.set_depth_unit(file.tracks.depth_track.depth_unit)

    yuv_frames = []
    depth_frames = []
    frame_mapper = rgbd.FrameMapper(file.attachments.camera_calibration, standard_calibration)
    color_decoder = rgbd.ColorDecoder(rgbd.ColorCodecType.VP8)
    for video_frame in file.video_frames:
        color_bytes = video_frame.color_bytes
        yuv_frame = color_decoder.decode(color_bytes)
        mapped_color_frame = frame_mapper.map_color_frame(yuv_frame)
        yuv_frames.append(mapped_color_frame)

    depth_decoder = rgbd.DepthDecoder(rgbd.DepthCodecType.TDC1)
    for video_frame in file.video_frames:
        depth_frame = depth_decoder.decode(video_frame.depth_bytes)
        mapped_depth_frame = frame_mapper.map_depth_frame(depth_frame)
        depth_frames.append(mapped_depth_frame)

    # cv2.imshow("color", rgb)
    # cv2.imshow("depth", depth_arrays[0].astype(np.uint16))

    color_width = yuv_frames[0].width
    color_height = yuv_frames[0].height
    depth_width = depth_frames[0].width
    depth_height = depth_frames[0].height

    print(f"color_width: {color_width}, color_height: {color_height}")
    print(f"depth_width: {depth_width}, depth_height: {depth_height}")

    # file_bytes_builder.set_cover_png_bytes(
    #     yuv_frames[0].get_mkv_cover_sized().get_png_bytes()
    # )
    file_bytes_builder.set_cover_png_bytes(file.attachments.cover_png_bytes)

    with rgbd.NativeColorEncoder(
        rgbd.lib.RGBD_COLOR_CODEC_TYPE_VP8, color_width, color_height, 30
    ) as color_encoder, rgbd.NativeDepthEncoder.create_tdc1_encoder(
        depth_width, depth_height, 500
    ) as depth_encoder:
        for index in range(len(file.video_frames)):
            print(f"index: {index}")
            video_frame = file.video_frames[index]
            keyframe = index % 60 == 0

            yuv_frame = yuv_frames[index]
            depth_frame = depth_frames[index]
            color_bytes = color_encoder.encode(yuv_frame, keyframe)
            depth_bytes = depth_encoder.encode(depth_frame.values, keyframe)

            file_bytes_builder.add_video_frame(
                rgbd.FileVideoFrame(
                    video_frame.time_point_us, keyframe, color_bytes, depth_bytes
                )
            )

    for audio_frame in file.audio_frames:
        file_bytes_builder.add_audio_frame(audio_frame)

    previous_rotation = glm.quat(1, 0, 0, 0)
    previous_time_point_us = 0
    if len(file.imu_frames) > 0:
        previous_time_point_us = file.imu_frames[0].time_point_us

    for imu_frame in file.imu_frames:
        file_bytes_builder.add_imu_frame(imu_frame)

        delta_time_sec = (imu_frame.time_point_us - previous_time_point_us) / 1000.0 / 1000.0
        rotation = rgbd.apply_rotation_rate_and_gravity_to_rotation(previous_rotation,
                                                                    delta_time_sec,
                                                                    imu_frame.rotation_rate,
                                                                    imu_frame.gravity)
        print(f"imu to rotation: {rotation}")
        file_bytes_builder.add_trs_frame(rgbd.FileTRSFrame(imu_frame.time_point_us,
                                                           glm.vec3(0, 0, 0),
                                                           rotation,
                                                           glm.vec3(1, 1, 1)))
        previous_rotation = rotation
        previous_time_point_us = imu_frame.time_point_us

    # for trs_frame in file.trs_frames:
    #     file_bytes_builder.add_trs_frame(trs_frame)

    file_bytes_builder.build_to_path("tmp/test_encoder_result.mkv")


if __name__ == "__main__":
    main()
