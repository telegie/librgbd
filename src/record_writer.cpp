#include "record_writer.hpp"

#include "png_utils.hpp"

using namespace LIBMATROSKA_NAMESPACE;

namespace rgbd
{
Bytes convert_vec3_to_bytes(const glm::vec3& v)
{
    Bytes bytes;
    append_bytes(bytes, convert_to_bytes(v.x));
    append_bytes(bytes, convert_to_bytes(v.y));
    append_bytes(bytes, convert_to_bytes(v.z));
    return bytes;
}

Bytes convert_quat_to_bytes(const glm::quat& q)
{
    Bytes bytes;
    append_bytes(bytes, convert_to_bytes(q.w));
    append_bytes(bytes, convert_to_bytes(q.x));
    append_bytes(bytes, convert_to_bytes(q.y));
    append_bytes(bytes, convert_to_bytes(q.z));
    return bytes;
}

RecordWriter::RecordWriter(IOCallback& io_callback,
                       int sample_rate,
                       DepthCodecType depth_codec_type,
                       float depth_unit,
                       const CameraCalibration& calibration,
                       const optional<Bytes>& cover_png_bytes)
    : io_callback_{io_callback}
    , segment_{}
    , seek_head_placeholder_{}
    , segment_info_placeholder_{}
    , writer_tracks_{}
    , past_color_block_blob_{nullptr}
    , past_depth_block_blob_{nullptr}
    , last_timecode_{0}
{
    std::random_device random_device;
    std::mt19937 generator{random_device()};
    std::uniform_int_distribution<uint64_t> distribution{std::numeric_limits<uint64_t>::min(),
                                                         std::numeric_limits<uint64_t>::max()};
    //
    // init segment_info
    //
    {
        auto& segment_info{GetChild<KaxInfo>(segment_)};
        GetChild<KaxTimecodeScale>(segment_info).SetValue(MATROSKA_TIMESCALE_NS);
        GetChild<KaxMuxingApp>(segment_info).SetValue(L"libmatroska-1.6.3");
        GetChild<KaxWritingApp>(segment_info)
            .SetValueUTF8(fmt::format(
                "librgbd-{}.{}.{}", rgbd::MAJOR_VERSION, rgbd::MINOR_VERSION, rgbd::PATCH_VERSION));
        GetChild<KaxDateUTC>(segment_info).SetEpochDate(time(0));
        GetChild<KaxTitle>(segment_info).SetValue(L"Telegie Video");
    }

    constexpr uint64_t COLOR_TRACK_NUMBER{1};
    constexpr uint64_t DEPTH_TRACK_NUMBER{2};
    constexpr uint64_t AUDIO_TRACK_NUMBER{3};
    constexpr uint64_t ACCELERATION_TRACK_NUMBER{4};
    constexpr uint64_t ROTATION_RATE_TRACK_NUMBER{5};
    constexpr uint64_t MAGNETIC_FIELD_TRACK_NUMBER{6};
    constexpr uint64_t GRAVITY_TRACK_NUMBER{7};
    constexpr uint64_t TRANSLATION_TRACK_NUMBER{8};
    constexpr uint64_t ROTATION_TRACK_NUMBER{9};
    constexpr uint64_t CALIBRATION_TRACK_NUMBER{10};
    //
    // init color_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.color_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.color_track); // Track will be freed when the file is closed.
        writer_tracks_.color_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        // Track numbers start at 1
        GetChild<KaxTrackNumber>(*writer_tracks_.color_track).SetValue(COLOR_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.color_track).SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.color_track).SetValue(track_video);
        GetChild<KaxTrackName>(*writer_tracks_.color_track).SetValueUTF8("COLOR");
        GetChild<KaxCodecID>(*writer_tracks_.color_track).SetValue("V_VP8");

        GetChild<KaxTrackDefaultDuration>(*writer_tracks_.color_track)
            .SetValue(ONE_SECOND_NS / VIDEO_FRAME_RATE);
        auto& color_video_track{GetChild<KaxTrackVideo>(*writer_tracks_.color_track)};
        GetChild<KaxVideoPixelWidth>(color_video_track).SetValue(calibration.getColorWidth());
        GetChild<KaxVideoPixelHeight>(color_video_track).SetValue(calibration.getColorHeight());
    }
    //
    // init depth_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.depth_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.depth_track); // Track will be freed when the file is closed.
        writer_tracks_.depth_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.depth_track).SetValue(DEPTH_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.depth_track).SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.depth_track).SetValue(track_video);
        GetChild<KaxTrackName>(*writer_tracks_.depth_track).SetValueUTF8("DEPTH");
        if (depth_codec_type == DepthCodecType::RVL) {
            GetChild<KaxCodecID>(*writer_tracks_.depth_track).SetValue("V_RVL");
        } else if (depth_codec_type == DepthCodecType::TDC1) {
            GetChild<KaxCodecID>(*writer_tracks_.depth_track).SetValue("V_TDC1");
        } else {
            spdlog::error("Invalid depth codec found");
            throw std::runtime_error("Invalid depth codec found");
        }

        GetChild<KaxTrackDefaultDuration>(*writer_tracks_.depth_track)
            .SetValue(ONE_SECOND_NS / VIDEO_FRAME_RATE);

        json codec_private_json{{"depthUnit", depth_unit}};
        string codec_private_str{codec_private_json.dump()};
        std::vector<char> codec_private_vector(codec_private_str.begin(), codec_private_str.end());
        GetChild<KaxCodecPrivate>(*writer_tracks_.depth_track)
            .CopyBuffer(reinterpret_cast<binary*>(codec_private_vector.data()),
                        gsl::narrow<uint32_t>(codec_private_vector.size()));
        // spdlog::info("FileWriter codec_private_str: {}", codec_private_str);

        auto& depth_video_track{GetChild<KaxTrackVideo>(*writer_tracks_.depth_track)};
        GetChild<KaxVideoPixelWidth>(depth_video_track).SetValue(calibration.getDepthWidth());
        GetChild<KaxVideoPixelHeight>(depth_video_track).SetValue(calibration.getDepthHeight());
    }
    //
    // init audio_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.audio_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.audio_track); // Track will be freed when the file is closed.
        writer_tracks_.audio_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.audio_track).SetValue(AUDIO_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.audio_track).SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.audio_track).SetValue(track_audio);
        GetChild<KaxTrackName>(*writer_tracks_.audio_track).SetValueUTF8("AUDIO");
        GetChild<KaxCodecID>(*writer_tracks_.audio_track).SetValue("A_OPUS");

        // GetChild<KaxCodecDelay>(*audio_track).SetValue(6500000); // from a file from ffmpeg-based
        // recorder
        constexpr uint64_t OPUS_SEEK_PREROLL{80000000}; // from ffmpeg matroskaenc.c
        GetChild<KaxSeekPreRoll>(*writer_tracks_.audio_track).SetValue(OPUS_SEEK_PREROLL);

        // reference: https://wiki.xiph.org/MatroskaOpus
        // CodecPriavte struct from: https://datatracker.ietf.org/doc/html/rfc7845
        Bytes opus_head_bytes;
        append_bytes(opus_head_bytes, convert_to_bytes('O'));
        append_bytes(opus_head_bytes, convert_to_bytes('p'));
        append_bytes(opus_head_bytes, convert_to_bytes('u'));
        append_bytes(opus_head_bytes, convert_to_bytes('s'));
        append_bytes(opus_head_bytes, convert_to_bytes('H'));
        append_bytes(opus_head_bytes, convert_to_bytes('e'));
        append_bytes(opus_head_bytes, convert_to_bytes('a'));
        append_bytes(opus_head_bytes, convert_to_bytes('d'));
        uint8_t version{1};
        append_bytes(opus_head_bytes, convert_to_bytes(version));
        uint8_t channel_count{1};
        append_bytes(opus_head_bytes, convert_to_bytes(channel_count));
        uint16_t preskip{3840};
        append_bytes(opus_head_bytes, convert_to_bytes(preskip));
        uint32_t intput_samplerate{gsl::narrow<uint32_t>(sample_rate)};
        append_bytes(opus_head_bytes, convert_to_bytes(intput_samplerate));
        uint16_t output_gain{0};
        append_bytes(opus_head_bytes, convert_to_bytes(output_gain));
        uint8_t channel_mapping_family{0};
        append_bytes(opus_head_bytes, convert_to_bytes(channel_mapping_family));

        constexpr int OPUS_HEAD_SIZE{19};
        if (opus_head_bytes.size() != OPUS_HEAD_SIZE)
            throw std::runtime_error("opus_head_bytes.size() != OPUS_HEAD_SIZE");

        binary* opus_head{new binary[OPUS_HEAD_SIZE]};
        memcpy(opus_head, opus_head_bytes.data(), OPUS_HEAD_SIZE);
        GetChild<KaxCodecPrivate>(*writer_tracks_.audio_track).SetBuffer(opus_head, OPUS_HEAD_SIZE);

        // KaxTrackDefaultDuration expects "number of nanoseconds (not scaled) per frame" here.
        GetChild<KaxTrackDefaultDuration>(*writer_tracks_.audio_track)
            .SetValue(ONE_SECOND_NS / sample_rate);
        auto& audio_track_details = GetChild<KaxTrackAudio>(*writer_tracks_.audio_track);
        GetChild<KaxAudioSamplingFreq>(audio_track_details).SetValue(sample_rate);
        GetChild<KaxAudioOutputSamplingFreq>(audio_track_details).SetValue(sample_rate);
        GetChild<KaxAudioChannels>(audio_track_details).SetValue(AUDIO_INPUT_CHANNEL_COUNT);
        GetChild<KaxAudioBitDepth>(audio_track_details).SetValue(sizeof(float) * 8);
    }
    //
    // init acceleration_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.acceleration_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.acceleration_track); // Track will be freed when the file is closed.
        writer_tracks_.acceleration_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.acceleration_track)
            .SetValue(ACCELERATION_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.acceleration_track)
            .SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.acceleration_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.acceleration_track).SetValueUTF8("ACCELERATION");
        GetChild<KaxCodecID>(*writer_tracks_.acceleration_track).SetValue("S_ACCELERATION");
    }
    //
    // init rotation_rate_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.rotation_rate_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.rotation_rate_track); // Track will be freed when the file is closed.
        writer_tracks_.rotation_rate_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.rotation_rate_track)
            .SetValue(ROTATION_RATE_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.rotation_rate_track)
            .SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.rotation_rate_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.rotation_rate_track).SetValueUTF8("ROTATION_RATE");
        GetChild<KaxCodecID>(*writer_tracks_.rotation_rate_track).SetValue("S_ROTATION_RATE");
    }
    //
    // init magnetic_field_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.magnetic_field_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.magnetic_field_track); // Track will be freed when the file is closed.
        writer_tracks_.magnetic_field_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.magnetic_field_track)
            .SetValue(MAGNETIC_FIELD_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.magnetic_field_track)
            .SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.magnetic_field_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.magnetic_field_track).SetValueUTF8("MAGNETIC_FIELD");
        GetChild<KaxCodecID>(*writer_tracks_.magnetic_field_track).SetValue("S_MAGNETIC_FIELD");
    }
    //
    // init gravity_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.gravity_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.gravity_track); // Track will be freed when the file is closed.
        writer_tracks_.gravity_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.gravity_track).SetValue(GRAVITY_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.gravity_track).SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.gravity_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.gravity_track).SetValueUTF8("GRAVITY");
        GetChild<KaxCodecID>(*writer_tracks_.gravity_track).SetValue("S_GRAVITY");
    }
    //
    // init translation_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.translation_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.translation_track); // Track will be freed when the file is closed.
        writer_tracks_.translation_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.translation_track)
            .SetValue(TRANSLATION_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.translation_track)
            .SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.translation_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.translation_track).SetValueUTF8("TRANSLATION");
        GetChild<KaxCodecID>(*writer_tracks_.translation_track).SetValue("S_TRANSLATION");
    }
    //
    // init rotation_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.rotation_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.rotation_track); // Track will be freed when the file is closed.
        writer_tracks_.rotation_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.rotation_track).SetValue(ROTATION_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.rotation_track).SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.rotation_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.rotation_track).SetValueUTF8("ROTATION");
        GetChild<KaxCodecID>(*writer_tracks_.rotation_track).SetValue("S_ROTATION");
    }
    //
    // init calibration_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        writer_tracks_.calibration_track = new KaxTrackEntry;
        tracks.PushElement(
            *writer_tracks_.calibration_track); // Track will be freed when the file is closed.
        writer_tracks_.calibration_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.calibration_track).SetValue(CALIBRATION_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.calibration_track).SetValue(distribution(generator));
        GetChild<KaxTrackType>(*writer_tracks_.calibration_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.calibration_track).SetValueUTF8("CALIBRATION");
        GetChild<KaxCodecID>(*writer_tracks_.calibration_track).SetValue("S_CALIBRATION");
    }
    //
    // init calibration as attachment
    //
    {
        auto& attachments{GetChild<KaxAttachments>(segment_)};
        auto calibration_attached_file{new KaxAttached};
        attachments.PushElement(*calibration_attached_file);
        GetChild<KaxFileName>(*calibration_attached_file).SetValueUTF8("calibration.json");
        GetChild<KaxMimeType>(*calibration_attached_file).SetValue("application/octet-stream");
        GetChild<KaxFileUID>(*calibration_attached_file).SetValue(distribution(generator));

        string calibration_str{calibration.toJson().dump()};
        std::vector<char> calibration_chars(calibration_str.begin(), calibration_str.end());
        GetChild<KaxFileData>(*calibration_attached_file)
            .CopyBuffer(reinterpret_cast<binary*>(calibration_chars.data()),
                        gsl::narrow<uint32_t>(calibration_chars.size()));
    }
    //
    // init cover as attachment
    //
    if (cover_png_bytes)
    {
        auto& attachments{GetChild<KaxAttachments>(segment_)};
        auto cover_attached_file{new KaxAttached};
        attachments.PushElement(*cover_attached_file);
        GetChild<KaxFileName>(*cover_attached_file).SetValueUTF8("cover.png");
        GetChild<KaxMimeType>(*cover_attached_file).SetValue("image/png");
        GetChild<KaxFileUID>(*cover_attached_file).SetValue(distribution(generator));

        GetChild<KaxFileData>(*cover_attached_file)
            .CopyBuffer(reinterpret_cast<const binary*>(cover_png_bytes->data()),
                        gsl::narrow<uint32_t>(cover_png_bytes->size()));
    }
    //
    // init KaxCues
    //
    {
        auto& cues{GetChild<KaxCues>(segment_)};
        cues.SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);
    }

    //
    // write EbmlHead
    //
    {
        EbmlHead head;
        GetChild<EDocType>(head).SetValue("matroska");
        GetChild<EDocTypeVersion>(head).SetValue(2);
        GetChild<EDocTypeReadVersion>(head).SetValue(2);
        head.Render(io_callback_, true);
    }

    // size is unknown and will always be, we can render it right away
    segment_.WriteHead(io_callback_, 8);

    // Following the optimum layout of SeekHead -> Info -> Tracks -> Attachements -> Clusters.
    // reference: https://www.matroska.org/technical/ordering.html

    //
    // write placeholders
    //
    {
        // reserve some space for the Meta Seek writen at the end
        seek_head_placeholder_.SetSize(256);
        seek_head_placeholder_.Render(io_callback_);

        segment_info_placeholder_.SetSize(256);
        segment_info_placeholder_.Render(io_callback_);
    }

    // Write KaxTracks
    {
        auto& tracks{GetChild<KaxTracks>(segment_)};
        tracks.Render(io_callback_);
    }

    // Write KaxAttachments
    {
        auto& attachments{GetChild<KaxAttachments>(segment_)};
        attachments.Render(io_callback_);
    }
}

void RecordWriter::writeVideoFrame(const RecordVideoFrame& video_frame)
{
    int64_t time_point_ns{video_frame.time_point_us() * 1000};
    if (time_point_ns < 0) {
        spdlog::error("FileWriter::writeVideoFrame: time_point_ns ({}) should not be negative.",
                      time_point_ns);
        return;
    }

    auto& cues{GetChild<KaxCues>(segment_)};
    auto video_timecode{gsl::narrow<uint64_t>(time_point_ns)};

    auto video_cluster{new KaxCluster};
    segment_.PushElement(*video_cluster);
    video_cluster->InitTimecode(video_timecode / MATROSKA_TIMESCALE_NS, MATROSKA_TIMESCALE_NS);
    video_cluster->SetParent(segment_);
    video_cluster->EnableChecksum();

    auto color_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto color_data_buffer{new DataBuffer{const_cast<uint8_t*>(video_frame.color_bytes().data()),
        gsl::narrow<uint32_t>(video_frame.color_bytes().size())}};
    video_cluster->AddBlockBlob(color_block_blob);
    color_block_blob->SetParent(*video_cluster);
    color_block_blob->AddFrameAuto(*writer_tracks_.color_track,
                                   video_timecode,
                                   *color_data_buffer,
                                   LACING_AUTO,
                                   video_frame.keyframe() ? nullptr : past_color_block_blob_);

    auto depth_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto depth_data_buffer{new DataBuffer{const_cast<uint8_t*>(video_frame.depth_bytes().data()),
        gsl::narrow<uint32_t>(video_frame.depth_bytes().size())}};
    video_cluster->AddBlockBlob(depth_block_blob);
    depth_block_blob->SetParent(*video_cluster);
    depth_block_blob->AddFrameAuto(*writer_tracks_.depth_track,
                                   video_timecode,
                                   *depth_data_buffer,
                                   LACING_AUTO,
                                   video_frame.keyframe() ? nullptr : past_depth_block_blob_);

    video_cluster->Render(io_callback_, cues);
    video_cluster->ReleaseFrames();

    past_color_block_blob_ = color_block_blob;
    past_depth_block_blob_ = depth_block_blob;
    last_timecode_ = video_timecode;
}

void RecordWriter::writeAudioFrame(const RecordAudioFrame& audio_frame)
{
    int64_t time_point_ns{audio_frame.time_point_us() * 1000};
    if (time_point_ns < 0) {
        spdlog::error("FileWriter::writeAudioFrame: time_point_ns ({}) should be positive.",
                      time_point_ns);
        return;
    }

    auto audio_frame_timecode{gsl::narrow<uint64_t>(time_point_ns)};

    auto& cues{GetChild<KaxCues>(segment_)};
    auto audio_cluster_timecode{audio_frame_timecode};

    auto audio_cluster{new KaxCluster};
    segment_.PushElement(*audio_cluster);
    audio_cluster->InitTimecode(audio_cluster_timecode / MATROSKA_TIMESCALE_NS,
                                MATROSKA_TIMESCALE_NS);
    audio_cluster->SetParent(segment_);
    audio_cluster->EnableChecksum();

    auto block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    // const_cast is okay here since the audio_bytes will not be modified here,
    // and this lets the argument become a const one, which is helpful for the C API side.
    // For example, this makes calling the C API easier from Swift.
    auto data_buffer{new DataBuffer{const_cast<uint8_t*>(audio_frame.bytes().data()),
                                    gsl::narrow<uint32_t>(audio_frame.bytes().size())}};
    audio_cluster->AddBlockBlob(block_blob);
    block_blob->SetParent(*audio_cluster);
    block_blob->AddFrameAuto(*writer_tracks_.audio_track, audio_cluster_timecode, *data_buffer);

    audio_cluster->Render(io_callback_, cues);
    audio_cluster->ReleaseFrames();

    last_timecode_ = audio_cluster_timecode;
}

void RecordWriter::writeIMUFrame(const RecordIMUFrame& imu_frame)
{
    int64_t time_point_ns{imu_frame.time_point_us() * 1000};
    if (time_point_ns < 0) {
        spdlog::error("FileWriter::writeIMUFrame: time_point_ns ({}) should not be negative ({}).",
                      time_point_ns);
        return;
    }

    auto imu_timecode{gsl::narrow<uint64_t>(time_point_ns)};

    auto& cues{GetChild<KaxCues>(segment_)};

    auto imu_cluster{new KaxCluster};
    segment_.PushElement(*imu_cluster);
    imu_cluster->InitTimecode(imu_timecode / MATROSKA_TIMESCALE_NS, MATROSKA_TIMESCALE_NS);
    imu_cluster->SetParent(segment_);
    imu_cluster->EnableChecksum();

    Bytes acceleration_bytes(convert_vec3_to_bytes(imu_frame.acceleration()));

    auto acceleration_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto acceleration_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(acceleration_bytes.data()),
                       gsl::narrow<uint32_t>(acceleration_bytes.size())}};
    imu_cluster->AddBlockBlob(acceleration_block_blob);
    acceleration_block_blob->SetParent(*imu_cluster);
    acceleration_block_blob->AddFrameAuto(
        *writer_tracks_.acceleration_track, imu_timecode, *acceleration_data_buffer);

    Bytes rotation_rate_bytes(convert_vec3_to_bytes(imu_frame.rotation_rate()));

    auto rotation_rate_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto rotation_rate_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(rotation_rate_bytes.data()),
                       gsl::narrow<uint32_t>(rotation_rate_bytes.size())}};
    imu_cluster->AddBlockBlob(rotation_rate_block_blob);
    rotation_rate_block_blob->SetParent(*imu_cluster);
    rotation_rate_block_blob->AddFrameAuto(
        *writer_tracks_.rotation_rate_track, imu_timecode, *rotation_rate_data_buffer);

    Bytes magnetic_field_bytes(convert_vec3_to_bytes(imu_frame.magnetic_field()));

    auto magnetic_field_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto magnetic_field_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(magnetic_field_bytes.data()),
                       gsl::narrow<uint32_t>(magnetic_field_bytes.size())}};
    imu_cluster->AddBlockBlob(magnetic_field_block_blob);
    magnetic_field_block_blob->SetParent(*imu_cluster);
    magnetic_field_block_blob->AddFrameAuto(
        *writer_tracks_.magnetic_field_track, imu_timecode, *magnetic_field_data_buffer);

    Bytes gravity_bytes(convert_vec3_to_bytes(imu_frame.gravity()));

    auto gravity_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto gravity_data_buffer{new DataBuffer{reinterpret_cast<uint8_t*>(gravity_bytes.data()),
                                            gsl::narrow<uint32_t>(gravity_bytes.size())}};
    imu_cluster->AddBlockBlob(gravity_block_blob);
    gravity_block_blob->SetParent(*imu_cluster);
    gravity_block_blob->AddFrameAuto(
        *writer_tracks_.gravity_track, imu_timecode, *gravity_data_buffer);

    imu_cluster->Render(io_callback_, cues);
    imu_cluster->ReleaseFrames();

    last_timecode_ = imu_timecode;
}

void RecordWriter::writePoseFrame(const RecordPoseFrame& pose_frame)
{
    int64_t time_point_ns{pose_frame.time_point_us() * 1000};
    if (time_point_ns < 0) {
        spdlog::error("FileWriter::writePoseFrame: time_point_ns ({}) should not be negative.",
                      time_point_ns);
        return;
    }

    auto pose_timecode{gsl::narrow<uint64_t>(time_point_ns)};

    auto& cues{GetChild<KaxCues>(segment_)};

    auto pose_cluster{new KaxCluster};
    segment_.PushElement(*pose_cluster);
    pose_cluster->InitTimecode(pose_timecode / MATROSKA_TIMESCALE_NS, MATROSKA_TIMESCALE_NS);
    pose_cluster->SetParent(segment_);
    pose_cluster->EnableChecksum();

    Bytes translation_bytes(convert_vec3_to_bytes(pose_frame.translation()));

    auto translation_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto translation_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(translation_bytes.data()),
                       gsl::narrow<uint32_t>(translation_bytes.size())}};
    pose_cluster->AddBlockBlob(translation_block_blob);
    translation_block_blob->SetParent(*pose_cluster);
    translation_block_blob->AddFrameAuto(
        *writer_tracks_.translation_track, pose_timecode, *translation_data_buffer);

    Bytes rotation_bytes(convert_quat_to_bytes(pose_frame.rotation()));

    auto rotation_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto rotation_data_buffer{new DataBuffer{reinterpret_cast<uint8_t*>(rotation_bytes.data()),
                                             gsl::narrow<uint32_t>(rotation_bytes.size())}};
    pose_cluster->AddBlockBlob(rotation_block_blob);
    rotation_block_blob->SetParent(*pose_cluster);
    rotation_block_blob->AddFrameAuto(
        *writer_tracks_.rotation_track, pose_timecode, *rotation_data_buffer);

    pose_cluster->Render(io_callback_, cues);
    pose_cluster->ReleaseFrames();

    last_timecode_ = pose_timecode;
}

void RecordWriter::writeCalibrationFrame(const RecordCalibrationFrame& calibration_frame)
{
    int64_t time_point_ns{calibration_frame.time_point_us() * 1000};
    if (time_point_ns < 0) {
        spdlog::error("FileWriter::writeCalibrationFrame: time_point_ns ({}) should not be negative.",
                      time_point_ns);
        return;
    }

    auto calibration_timecode{gsl::narrow<uint64_t>(time_point_ns)};

    auto& cues{GetChild<KaxCues>(segment_)};

    auto calibration_cluster{new KaxCluster};
    segment_.PushElement(*calibration_cluster);
    calibration_cluster->InitTimecode(calibration_timecode / MATROSKA_TIMESCALE_NS, MATROSKA_TIMESCALE_NS);
    calibration_cluster->SetParent(segment_);
    calibration_cluster->EnableChecksum();

    string calibration_str{calibration_frame.camera_calibration()->toJson().dump()};
    std::vector<char> calibration_chars(calibration_str.begin(), calibration_str.end());

    auto calibration_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto calibration_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(calibration_chars.data()),
                       gsl::narrow<uint32_t>(calibration_chars.size())}};
    calibration_cluster->AddBlockBlob(calibration_block_blob);
    calibration_block_blob->SetParent(*calibration_cluster);
    calibration_block_blob->AddFrameAuto(
        *writer_tracks_.calibration_track, calibration_timecode, *calibration_data_buffer);

    calibration_cluster->Render(io_callback_, cues);
    calibration_cluster->ReleaseFrames();

    last_timecode_ = calibration_timecode;
}

void RecordWriter::flush()
{
    {
        auto duration{gsl::narrow<uint64_t>(last_timecode_ / MATROSKA_TIMESCALE_NS)};

        auto& segment_info{GetChild<KaxInfo>(segment_)};
        GetChild<KaxDuration>(segment_info).SetValue(duration);
        segment_info_placeholder_.ReplaceWith(segment_info, io_callback_);
    }

    //
    // render KaxCues
    //
    auto& cues{GetChild<KaxCues>(segment_)};
    cues.Render(io_callback_);

    //
    // update KaxSeekHead
    //
    {
        auto& seek_head{GetChild<KaxSeekHead>(segment_)};

        auto& segment_info{GetChild<KaxInfo>(segment_)};
        seek_head.IndexThis(segment_info, segment_);

        auto& tracks{GetChild<KaxTracks>(segment_)};
        seek_head.IndexThis(tracks, segment_);

        auto& attachments{GetChild<KaxAttachments>(segment_)};
        seek_head.IndexThis(attachments, segment_);

        seek_head.IndexThis(cues, segment_);

        seek_head_placeholder_.ReplaceWith(seek_head, io_callback_);
    }

    io_callback_.setFilePointer(0, libebml::seek_end);
    uint64_t segment_size{io_callback_.getFilePointer() - segment_.GetElementPosition() -
                          segment_.HeadSize()};
    segment_.SetSizeInfinite(true);
    if (!segment_.ForceSize(segment_size))
        spdlog::info("Failed to set segment size");
    segment_.OverwriteHead(io_callback_);
    io_callback_.close();
}
} // namespace rgbd
