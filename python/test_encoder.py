import pyrgbd as rgbd
import requests
import os.path
import glm


def main():
    print(f"TWO: {rgbd.get_number_two()}")
    print(f"MAJOR: {rgbd.get_librgbd_major_version()}")

    base64url_video_id = "A9oofdweNJ4"
    video_file_path = f"tmp/{base64url_video_id}.mkv"
    if not os.path.exists(video_file_path):
        video_id = rgbd.decode_base64url_to_long(base64url_video_id)
        video_url = f"https://posts.telegie.com/v1/{video_id}/{video_id}.mkv"
        response = requests.get(video_url)
        with open(video_file_path, "wb+") as file:
            file.write(response.content)

    with rgbd.NativeFileParser(video_file_path) as native_file_parser:
        with native_file_parser.parse(True, True) as native_file:
            file = rgbd.File(native_file)

    file_writer_helper = rgbd.FileWriterHelper()
    file_writer_helper.set_calibration(file.attachments.camera_calibration)

    color_track = file.tracks.color_track
    depth_track = file.tracks.depth_track
    print(f"depth_track.depth_unit: {depth_track.depth_unit}")

    # with rgbd.create_native_undistorted_camera_calibration(color_track.width, color_track.height,
    #                                                        depth_track.width, depth_track.height,
    #                                                        0.5, -1.0, 0.5, 0.5) as native_calibration:
    #     write_config = rgbd.NativeFileWriterConfig()
    #     write_config.set_depth_codec_type(rgbd.lib.RGBD_DEPTH_CODEC_TYPE_TDC1)
    #     file_writer = rgbd.NativeFileWriter("tmp/written_file.mkv",
    #                                         native_calibration,
    #                                         write_config)

    yuv_frames = []
    color_arrays = []
    with rgbd.NativeColorDecoder(rgbd.ColorCodecType.VP8) as native_color_decoder:
        for video_frame in file.video_frames:
            yuv_frame = native_color_decoder.decode(video_frame.color_bytes)
            color_array = rgbd.convert_yuv420_to_rgb(yuv_frame.y_channel, yuv_frame.u_channel, yuv_frame.v_channel)
            yuv_frames.append(yuv_frame)
            color_arrays.append(color_array)

    depth_frames = []
    with rgbd.NativeDepthDecoder(rgbd.DepthCodecType.TDC1) as depth_decoder:
        for video_frame in file.video_frames:
            depth_frame = depth_decoder.decode(video_frame.depth_bytes)
            depth_frames.append(depth_frame)

    # cv2.imshow("color", rgb)
    # cv2.imshow("depth", depth_arrays[0].astype(np.uint16))

    depth_width = depth_frames[0].width
    depth_height = depth_frames[0].height

    print(f"depth_width: {depth_width}, depth_height: {depth_height}")

    file_writer_helper.set_cover(yuv_frames[0])

    with rgbd.NativeColorEncoder(rgbd.lib.RGBD_COLOR_CODEC_TYPE_VP8,
                                 yuv_frame.width,
                                 yuv_frame.height,
                                 2500,
                                 30) as color_encoder, \
            rgbd.NativeDepthEncoder.create_tdc1_encoder(depth_width, depth_height, 500) as depth_encoder:
        for index in range(len(file.video_frames)):
            video_frame = file.video_frames[index]
            keyframe = index % 60 == 0

            yuv_frame = yuv_frames[index]
            depth_frame = depth_frames[index]
            color_bytes = color_encoder.encode(yuv_frame, keyframe)
            depth_bytes = depth_encoder.encode(depth_frame.values, keyframe)

            file_writer_helper.add_color_byte_frame(video_frame.time_point_us,
                                                    keyframe,
                                                    color_bytes)
            file_writer_helper.add_depth_byte_frame(video_frame.time_point_us,
                                                    keyframe,
                                                    depth_bytes)

    for audio_frame in file.audio_frames:
        file_writer_helper.add_audio_frame(audio_frame)

    for imu_frame in file.imu_frames:
        file_writer_helper.add_imu_frame(imu_frame)

    for trs_frame in file.trs_frames:
        file_writer_helper.add_trs_frame(trs_frame)

    file_writer_helper.write("tmp/written_file.mkv")


if __name__ == "__main__":
    main()
