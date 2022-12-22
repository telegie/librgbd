#include "file_writer.hpp"

#include <rgbd/png_utils.hpp>

using namespace LIBMATROSKA_NAMESPACE;

namespace rgbd
{
constexpr int64_t ONE_SECOND_NS{1000 * 1000 * 1000}; // in ns
constexpr int64_t ONE_MICROSECOND_NS{1000};          // in ns
constexpr int MATROSKA_TIMESCALE_NS{ONE_MICROSECOND_NS};

auto get_random_number()
{
    std::random_device random_device;
    return random_device();
}

void convert_yuv_to_rgb(int y_row,
                        int y_col,
                        int y_width,
                        gsl::span<const uint8_t> y_channel,
                        gsl::span<const uint8_t> u_channel,
                        gsl::span<const uint8_t> v_channel,
                        uint8_t& r,
                        uint8_t& g,
                        uint8_t& b)
{
    int uv_row{y_row / 2};
    int uv_col{y_col / 2};
    int uv_width{y_width / 2};
    uint8_t y{y_channel[y_row * y_width + y_col]};
    uint8_t u{u_channel[uv_row * uv_width + uv_col]};
    uint8_t v{v_channel[uv_row * uv_width + uv_col]};

    // from https://en.wikipedia.org/wiki/YUV
    int r_tmp = y + ((351 * (v - 128)) >> 8);
    int g_tmp = y - ((179 * (v - 128) + 86 * (u - 128)) >> 8);
    int b_tmp = y + ((443 * (u - 128)) >> 8);
    r = std::clamp(r_tmp, 0, 255);
    g = std::clamp(g_tmp, 0, 255);
    b = std::clamp(b_tmp, 0, 255);
}

void set_rgb_channels(uint8_t r,
                      uint8_t g,
                      uint8_t b,
                      int rgb_row,
                      int rgb_col,
                      int rgb_width,
                      vector<uint8_t>& r_channel,
                      vector<uint8_t>& g_channel,
                      vector<uint8_t>& b_channel)
{
    r_channel[rgb_row * rgb_width + rgb_col] = r;
    g_channel[rgb_row * rgb_width + rgb_col] = g;
    b_channel[rgb_row * rgb_width + rgb_col] = b;
}

Bytes get_cover_png_bytes(int width,
                          int height,
                          gsl::span<const uint8_t> y_channel,
                          gsl::span<const uint8_t> u_channel,
                          gsl::span<const uint8_t> v_channel)
{
    constexpr int COVER_SIZE{600};
    std::vector<uint8_t> r_channel(COVER_SIZE * COVER_SIZE, 0);
    std::vector<uint8_t> g_channel(COVER_SIZE * COVER_SIZE, 0);
    std::vector<uint8_t> b_channel(COVER_SIZE * COVER_SIZE, 0);
    std::vector<uint8_t> a_channel(COVER_SIZE * COVER_SIZE, 255);

    // Pick the largest centered square area.
    int y_row_start{0};
    int y_row_end{height};
    int y_col_start{0};
    int y_col_end{width};
    if (width > height) {
        y_col_start = width / 2 - height / 2;
        y_col_end = width / 2 + height / 2;
    } else {
        y_row_start = height / 2 - width / 2;
        y_row_end = height / 2 + width / 2;
    }

    for (int cover_row{0}; cover_row < COVER_SIZE; ++cover_row) {
        int y_row{y_row_start + cover_row * (y_row_end - y_row_start) / COVER_SIZE};
        for (int cover_col{0}; cover_col < COVER_SIZE; ++cover_col) {
            int y_col{y_col_start + cover_col * (y_col_end - y_col_start) / COVER_SIZE};
            uint8_t r, g, b;
            convert_yuv_to_rgb(y_row, y_col, width, y_channel, u_channel, v_channel, r, g, b);
            r_channel[cover_row * COVER_SIZE + cover_col] = r;
            g_channel[cover_row * COVER_SIZE + cover_col] = g;
            b_channel[cover_row * COVER_SIZE + cover_col] = b;
        }
    }

    return PNGUtils::write(COVER_SIZE, COVER_SIZE, r_channel, g_channel, b_channel, a_channel);
}

vector<byte> convert_vec3_to_bytes(const glm::vec3& v)
{
    vector<byte> bytes;
    append_bytes(bytes, convert_to_bytes(v.x));
    append_bytes(bytes, convert_to_bytes(v.y));
    append_bytes(bytes, convert_to_bytes(v.z));
    return bytes;
}

vector<byte> convert_quat_to_bytes(const glm::quat& q)
{
    vector<byte> bytes;
    append_bytes(bytes, convert_to_bytes(q.w));
    append_bytes(bytes, convert_to_bytes(q.x));
    append_bytes(bytes, convert_to_bytes(q.y));
    append_bytes(bytes, convert_to_bytes(q.z));
    return bytes;
}

FileWriter::FileWriter(const string& file_path,
                       const CameraCalibration& calibration,
                       const FileWriterConfig& config)
    : generator_{get_random_number()}
    , distribution_{std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()}
    , io_callback_{std::make_unique<StdIOCallback>(file_path.c_str(), MODE_CREATE)}
    , segment_{std::make_unique<KaxSegment>()}
    , writer_tracks_{}
    , seek_head_placeholder_{nullptr}
    , segment_info_placeholder_{nullptr}
    , initial_time_point_ns_{nullopt}
    , past_color_block_blob_{nullptr}
    , past_depth_block_blob_{nullptr}
    , last_timecode_{0}
{
    init(calibration, config);
}

FileWriter::FileWriter(const CameraCalibration& calibration, const FileWriterConfig& config)
    : generator_{get_random_number()}
    , distribution_{std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()}
    , io_callback_{std::make_unique<MemIOCallback>()}
    , segment_{std::make_unique<KaxSegment>()}
    , writer_tracks_{}
    , seek_head_placeholder_{nullptr}
    , segment_info_placeholder_{nullptr}
    , initial_time_point_ns_{nullopt}
    , past_color_block_blob_{nullptr}
    , past_depth_block_blob_{nullptr}
    , last_timecode_{0}
{
    init(calibration, config);
}

void FileWriter::init(const CameraCalibration& calibration,
                      const FileWriterConfig& config)
{
    //
    // init segment_info
    //
    {
        auto& segment_info{GetChild<KaxInfo>(*segment_)};
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
    constexpr uint64_t SCALE_TRACK_NUMBER{10};
    //
    // init color_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.color_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.color_track); // Track will be freed when the file is closed.
        writer_tracks_.color_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        // Track numbers start at 1
        GetChild<KaxTrackNumber>(*writer_tracks_.color_track).SetValue(COLOR_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.color_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.color_track).SetValue(track_video);
        GetChild<KaxTrackName>(*writer_tracks_.color_track).SetValueUTF8("COLOR");
        GetChild<KaxCodecID>(*writer_tracks_.color_track).SetValue("V_VP8");

        GetChild<KaxTrackDefaultDuration>(*writer_tracks_.color_track).SetValue(ONE_SECOND_NS / config.framerate);
        auto& color_video_track{GetChild<KaxTrackVideo>(*writer_tracks_.color_track)};
        GetChild<KaxVideoPixelWidth>(color_video_track).SetValue(calibration.getColorWidth());
        GetChild<KaxVideoPixelHeight>(color_video_track).SetValue(calibration.getColorHeight());
    }
    //
    // init depth_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.depth_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.depth_track); // Track will be freed when the file is closed.
        writer_tracks_.depth_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.depth_track).SetValue(DEPTH_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.depth_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.depth_track).SetValue(track_video);
        GetChild<KaxTrackName>(*writer_tracks_.depth_track).SetValueUTF8("DEPTH");
        if (config.depth_codec_type == DepthCodecType::RVL) {
            GetChild<KaxCodecID>(*writer_tracks_.depth_track).SetValue("V_RVL");
        } else if (config.depth_codec_type == DepthCodecType::TDC1) {
            GetChild<KaxCodecID>(*writer_tracks_.depth_track).SetValue("V_TDC1");
        } else {
            throw std::runtime_error("Invalid depth codec found");
        }

        GetChild<KaxTrackDefaultDuration>(*writer_tracks_.depth_track).SetValue(ONE_SECOND_NS / config.framerate);

        json codec_private_json{{"depthUnit", config.depth_unit}};
        string codec_private_str{codec_private_json.dump()};
        std::vector<char> codec_private_vector(codec_private_str.begin(), codec_private_str.end());
        GetChild<KaxCodecPrivate>(*writer_tracks_.depth_track)
            .CopyBuffer(reinterpret_cast<binary*>(codec_private_vector.data()),
                        gsl::narrow<uint32_t>(codec_private_vector.size()));
        spdlog::info("FileWriter codec_private_str: {}", codec_private_str);

        auto& depth_video_track{GetChild<KaxTrackVideo>(*writer_tracks_.depth_track)};
        GetChild<KaxVideoPixelWidth>(depth_video_track).SetValue(calibration.getDepthWidth());
        GetChild<KaxVideoPixelHeight>(depth_video_track).SetValue(calibration.getDepthHeight());
    }
    //
    // init audio_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.audio_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.audio_track); // Track will be freed when the file is closed.
        writer_tracks_.audio_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.audio_track).SetValue(AUDIO_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.audio_track).SetValue(distribution_(generator_));
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
        uint32_t intput_samplerate{gsl::narrow<uint32_t>(config.samplerate)};
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
            .SetValue(ONE_SECOND_NS / config.samplerate);
        auto& audio_track_details = GetChild<KaxTrackAudio>(*writer_tracks_.audio_track);
        GetChild<KaxAudioSamplingFreq>(audio_track_details).SetValue(config.samplerate);
        GetChild<KaxAudioOutputSamplingFreq>(audio_track_details).SetValue(config.samplerate);
        GetChild<KaxAudioChannels>(audio_track_details).SetValue(AUDIO_INPUT_CHANNEL_COUNT);
        GetChild<KaxAudioBitDepth>(audio_track_details).SetValue(sizeof(float) * 8);
    }
    //
    // init acceleration_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.acceleration_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.acceleration_track); // Track will be freed when the file is closed.
        writer_tracks_.acceleration_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.acceleration_track).SetValue(ACCELERATION_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.acceleration_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.acceleration_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.acceleration_track).SetValueUTF8("ACCELERATION");
        GetChild<KaxCodecID>(*writer_tracks_.acceleration_track).SetValue("S_ACCELERATION");
    }
    //
    // init rotation_rate_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.rotation_rate_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.rotation_rate_track); // Track will be freed when the file is closed.
        writer_tracks_.rotation_rate_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.rotation_rate_track).SetValue(ROTATION_RATE_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.rotation_rate_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.rotation_rate_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.rotation_rate_track).SetValueUTF8("ROTATION_RATE");
        GetChild<KaxCodecID>(*writer_tracks_.rotation_rate_track).SetValue("S_ROTATION_RATE");
    }
    //
    // init magnetic_field_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.magnetic_field_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.magnetic_field_track); // Track will be freed when the file is closed.
        writer_tracks_.magnetic_field_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.magnetic_field_track).SetValue(MAGNETIC_FIELD_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.magnetic_field_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.magnetic_field_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.magnetic_field_track).SetValueUTF8("MAGNETIC_FIELD");
        GetChild<KaxCodecID>(*writer_tracks_.magnetic_field_track).SetValue("S_MAGNETIC_FIELD");
    }
    //
    // init gravity_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.gravity_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.gravity_track); // Track will be freed when the file is closed.
        writer_tracks_.gravity_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.gravity_track).SetValue(GRAVITY_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.gravity_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.gravity_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.gravity_track).SetValueUTF8("GRAVITY");
        GetChild<KaxCodecID>(*writer_tracks_.gravity_track).SetValue("S_GRAVITY");
    }
    //
    // init translation_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.translation_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.translation_track); // Track will be freed when the file is closed.
        writer_tracks_.translation_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.translation_track).SetValue(TRANSLATION_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.translation_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.translation_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.translation_track).SetValueUTF8("TRANSLATION");
        GetChild<KaxCodecID>(*writer_tracks_.translation_track).SetValue("S_TRANSLATION");
    }
    //
    // init rotation_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.rotation_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.rotation_track); // Track will be freed when the file is closed.
        writer_tracks_.rotation_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.rotation_track).SetValue(ROTATION_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.rotation_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.rotation_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.rotation_track).SetValueUTF8("ROTATION");
        GetChild<KaxCodecID>(*writer_tracks_.rotation_track).SetValue("S_ROTATION");
    }
    //
    // init scale_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        writer_tracks_.scale_track = new KaxTrackEntry;
        tracks.PushElement(*writer_tracks_.scale_track); // Track will be freed when the file is closed.
        writer_tracks_.scale_track->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*writer_tracks_.scale_track).SetValue(SCALE_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*writer_tracks_.scale_track).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*writer_tracks_.scale_track).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*writer_tracks_.scale_track).SetValueUTF8("SCALE");
        GetChild<KaxCodecID>(*writer_tracks_.scale_track).SetValue("S_SCALE");
    }
    //
    // init KaxAttached
    //
    {
        auto& attachments{GetChild<KaxAttachments>(*segment_)};
        auto calibration_attached_file{new KaxAttached};
        attachments.PushElement(*calibration_attached_file);
        GetChild<KaxFileName>(*calibration_attached_file).SetValueUTF8("calibration.json");
        GetChild<KaxMimeType>(*calibration_attached_file).SetValue("application/octet-stream");
        GetChild<KaxFileUID>(*calibration_attached_file).SetValue(distribution_(generator_));

        string calibration_str{calibration.toJson().dump()};
        std::vector<char> calibration_vector(calibration_str.begin(), calibration_str.end());
        GetChild<KaxFileData>(*calibration_attached_file)
            .CopyBuffer(reinterpret_cast<binary*>(calibration_vector.data()),
                        gsl::narrow<uint32_t>(calibration_vector.size()));
    }
    //
    // init KaxCues
    //
    {
        auto& cues{GetChild<KaxCues>(*segment_)};
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
        head.Render(*io_callback_, true);
    }

    // size is unknown and will always be, we can render it right away
    segment_->WriteHead(*io_callback_, 8);

    // Following the optimum layout of SeekHead -> Info -> Tracks -> Attachements -> Clusters.
    // reference: https://www.matroska.org/technical/ordering.html

    //
    // write placeholders
    //
    {
        // reserve some space for the Meta Seek writen at the end
        seek_head_placeholder_ = std::make_unique<EbmlVoid>();
        seek_head_placeholder_->SetSize(256);
        seek_head_placeholder_->Render(*io_callback_);

        segment_info_placeholder_ = std::make_unique<EbmlVoid>();
        segment_info_placeholder_->SetSize(256);
        segment_info_placeholder_->Render(*io_callback_);
    }

    // Write KaxTracks
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        tracks.Render(*io_callback_);
    }
}

void FileWriter::writeCover(const YuvFrame& yuv_frame)
{
    writeCover(yuv_frame.width(),
               yuv_frame.height(),
               yuv_frame.y_channel(),
               yuv_frame.u_channel(),
               yuv_frame.v_channel());
}

void FileWriter::writeCover(int width,
                            int height,
                            gsl::span<const uint8_t> y_channel,
                            gsl::span<const uint8_t> u_channel,
                            gsl::span<const uint8_t> v_channel)
{
    //
    // add cover art
    //
    auto& attachments{GetChild<KaxAttachments>(*segment_)};
    auto cover_attached_file{new KaxAttached};
    attachments.PushElement(*cover_attached_file);
    GetChild<KaxFileName>(*cover_attached_file).SetValueUTF8("cover.png");
    GetChild<KaxMimeType>(*cover_attached_file).SetValue("image/png");
    GetChild<KaxFileUID>(*cover_attached_file).SetValue(distribution_(generator_));

    auto png_bytes{get_cover_png_bytes(width, height, y_channel, u_channel, v_channel)};
    GetChild<KaxFileData>(*cover_attached_file)
        .CopyBuffer(reinterpret_cast<binary*>(png_bytes.data()),
                    gsl::narrow<uint32_t>(png_bytes.size()));

    // Write KaxAttachments
    attachments.Render(*io_callback_);
}

void FileWriter::writeVideoFrame(int64_t time_point_us,
                                 bool keyframe,
                                 gsl::span<const byte> color_bytes,
                                 gsl::span<const byte> depth_bytes)
{
    int64_t time_point_ns{time_point_us * 1000};
    if (!initial_time_point_ns_)
        initial_time_point_ns_ = time_point_ns;

    auto& cues{GetChild<KaxCues>(*segment_)};
    auto video_timecode{gsl::narrow<uint64_t>(time_point_ns - *initial_time_point_ns_)};

    auto video_cluster{new KaxCluster};
    segment_->PushElement(*video_cluster);
    video_cluster->InitTimecode(video_timecode / MATROSKA_TIMESCALE_NS, MATROSKA_TIMESCALE_NS);
    video_cluster->SetParent(*segment_);
    video_cluster->EnableChecksum();

    auto color_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto color_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(const_cast<byte*>(color_bytes.data())),
                       gsl::narrow<uint32_t>(color_bytes.size())}};
    video_cluster->AddBlockBlob(color_block_blob);
    color_block_blob->SetParent(*video_cluster);
    color_block_blob->AddFrameAuto(*writer_tracks_.color_track,
                                   video_timecode,
                                   *color_data_buffer,
                                   LACING_AUTO,
                                   keyframe ? nullptr : past_color_block_blob_);

    auto depth_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto depth_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(const_cast<byte*>(depth_bytes.data())),
                       gsl::narrow<uint32_t>(depth_bytes.size())}};
    video_cluster->AddBlockBlob(depth_block_blob);
    depth_block_blob->SetParent(*video_cluster);
    depth_block_blob->AddFrameAuto(*writer_tracks_.depth_track,
                                   video_timecode,
                                   *depth_data_buffer,
                                   LACING_AUTO,
                                   keyframe ? nullptr : past_depth_block_blob_);

    video_cluster->Render(*io_callback_, cues);
    video_cluster->ReleaseFrames();

    past_color_block_blob_ = color_block_blob;
    past_depth_block_blob_ = depth_block_blob;
    last_timecode_ = video_timecode;
}

void FileWriter::writeAudioFrame(int64_t time_point_us, gsl::span<const std::byte> audio_bytes)
{
    int64_t time_point_ns{time_point_us * 1000};
    if (!initial_time_point_ns_)
        initial_time_point_ns_ = time_point_ns;

    auto audio_frame_timecode{gsl::narrow<uint64_t>(time_point_ns - *initial_time_point_ns_)};

    auto& cues{GetChild<KaxCues>(*segment_)};
    auto audio_cluster_timecode{audio_frame_timecode};

    auto audio_cluster{new KaxCluster};
    segment_->PushElement(*audio_cluster);
    audio_cluster->InitTimecode(audio_cluster_timecode / MATROSKA_TIMESCALE_NS,
                                MATROSKA_TIMESCALE_NS);
    audio_cluster->SetParent(*segment_);
    audio_cluster->EnableChecksum();

    auto block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    // const_cast is okay here since the audio_bytes will not be modified here,
    // and this lets the argument become a const one, which is helpful for the C API side.
    // For example, this makes calling the C API easier from Swift.
    auto data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(const_cast<byte*>(audio_bytes.data())),
                       gsl::narrow<uint32_t>(audio_bytes.size())}};
    audio_cluster->AddBlockBlob(block_blob);
    block_blob->SetParent(*audio_cluster);
    block_blob->AddFrameAuto(*writer_tracks_.audio_track, audio_cluster_timecode, *data_buffer);

    audio_cluster->Render(*io_callback_, cues);
    audio_cluster->ReleaseFrames();

    last_timecode_ = audio_cluster_timecode;
}

void FileWriter::writeAudioFrame(const FileAudioFrame& audio_frame)
{
    writeAudioFrame(audio_frame.global_timecode(), audio_frame.bytes());
}

void FileWriter::writeIMUFrame(int64_t time_point_us,
                               const glm::vec3& acceleration,
                               const glm::vec3& rotation_rate,
                               const glm::vec3& magnetic_field,
                               const glm::vec3& gravity)
{
    int64_t time_point_ns{time_point_us * 1000};
    if (!initial_time_point_ns_)
        initial_time_point_ns_ = time_point_ns;

    auto imu_timecode{gsl::narrow<uint64_t>(time_point_ns - *initial_time_point_ns_)};

    auto& cues{GetChild<KaxCues>(*segment_)};

    auto imu_cluster{new KaxCluster};
    segment_->PushElement(*imu_cluster);
    imu_cluster->InitTimecode(imu_timecode / MATROSKA_TIMESCALE_NS, MATROSKA_TIMESCALE_NS);
    imu_cluster->SetParent(*segment_);
    imu_cluster->EnableChecksum();

    vector<byte> acceleration_bytes(convert_vec3_to_bytes(acceleration));

    auto acceleration_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto acceleration_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(acceleration_bytes.data()),
                       gsl::narrow<uint32_t>(acceleration_bytes.size())}};
    imu_cluster->AddBlockBlob(acceleration_block_blob);
    acceleration_block_blob->SetParent(*imu_cluster);
    acceleration_block_blob->AddFrameAuto(
        *writer_tracks_.acceleration_track, imu_timecode, *acceleration_data_buffer);

    vector<byte> rotation_rate_bytes(convert_vec3_to_bytes(rotation_rate));

    auto rotation_rate_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto rotation_rate_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(rotation_rate_bytes.data()),
                       gsl::narrow<uint32_t>(rotation_rate_bytes.size())}};
    imu_cluster->AddBlockBlob(rotation_rate_block_blob);
    rotation_rate_block_blob->SetParent(*imu_cluster);
    rotation_rate_block_blob->AddFrameAuto(
        *writer_tracks_.rotation_rate_track, imu_timecode, *rotation_rate_data_buffer);

    vector<byte> magnetic_field_bytes(convert_vec3_to_bytes(magnetic_field));

    auto magnetic_field_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto magnetic_field_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(magnetic_field_bytes.data()),
                       gsl::narrow<uint32_t>(magnetic_field_bytes.size())}};
    imu_cluster->AddBlockBlob(magnetic_field_block_blob);
    magnetic_field_block_blob->SetParent(*imu_cluster);
    magnetic_field_block_blob->AddFrameAuto(
        *writer_tracks_.magnetic_field_track, imu_timecode, *magnetic_field_data_buffer);

    vector<byte> gravity_bytes(convert_vec3_to_bytes(gravity));

    auto gravity_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto gravity_data_buffer{new DataBuffer{reinterpret_cast<uint8_t*>(gravity_bytes.data()),
                                            gsl::narrow<uint32_t>(gravity_bytes.size())}};
    imu_cluster->AddBlockBlob(gravity_block_blob);
    gravity_block_blob->SetParent(*imu_cluster);
    gravity_block_blob->AddFrameAuto(*writer_tracks_.gravity_track, imu_timecode, *gravity_data_buffer);

    imu_cluster->Render(*io_callback_, cues);
    imu_cluster->ReleaseFrames();

    last_timecode_ = imu_timecode;
}

void FileWriter::writeIMUFrame(const FileIMUFrame& imu_frame)
{
    writeIMUFrame(imu_frame.global_timecode(),
                  imu_frame.acceleration(),
                  imu_frame.rotation_rate(),
                  imu_frame.magnetic_field(),
                  imu_frame.gravity());
}

void FileWriter::writeTRSFrame(int64_t time_point_us,
                               const glm::vec3& translation,
                               const glm::quat& rotation,
                               const glm::vec3& scale)
{
    int64_t time_point_ns{time_point_us * 1000};
    if (!initial_time_point_ns_)
        initial_time_point_ns_ = time_point_ns;

    auto trs_timecode{gsl::narrow<uint64_t>(time_point_ns - *initial_time_point_ns_)};

    auto& cues{GetChild<KaxCues>(*segment_)};

    auto trs_cluster{new KaxCluster};
    segment_->PushElement(*trs_cluster);
    trs_cluster->InitTimecode(trs_timecode / MATROSKA_TIMESCALE_NS, MATROSKA_TIMESCALE_NS);
    trs_cluster->SetParent(*segment_);
    trs_cluster->EnableChecksum();

    vector<byte> translation_bytes(convert_vec3_to_bytes(translation));

    auto translation_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto translation_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(translation_bytes.data()),
                       gsl::narrow<uint32_t>(translation_bytes.size())}};
    trs_cluster->AddBlockBlob(translation_block_blob);
    translation_block_blob->SetParent(*trs_cluster);
    translation_block_blob->AddFrameAuto(
        *writer_tracks_.translation_track, trs_timecode, *translation_data_buffer);

    vector<byte> rotation_bytes(convert_quat_to_bytes(rotation));

    auto rotation_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto rotation_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(rotation_bytes.data()),
                       gsl::narrow<uint32_t>(rotation_bytes.size())}};
    trs_cluster->AddBlockBlob(rotation_block_blob);
    rotation_block_blob->SetParent(*trs_cluster);
    rotation_block_blob->AddFrameAuto(
        *writer_tracks_.rotation_track, trs_timecode, *rotation_data_buffer);

    vector<byte> scale_bytes(convert_vec3_to_bytes(scale));

    auto scale_block_blob{new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE)};
    auto scale_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(scale_bytes.data()),
                       gsl::narrow<uint32_t>(scale_bytes.size())}};
    trs_cluster->AddBlockBlob(scale_block_blob);
    scale_block_blob->SetParent(*trs_cluster);
    scale_block_blob->AddFrameAuto(
        *writer_tracks_.scale_track, trs_timecode, *scale_data_buffer);

    trs_cluster->Render(*io_callback_, cues);
    trs_cluster->ReleaseFrames();

    last_timecode_ = trs_timecode;
}

void FileWriter::writeTRSFrame(const FileTRSFrame& trs_frame)
{
    writeTRSFrame(trs_frame.global_timecode(),
                  trs_frame.translation(),
                  trs_frame.rotation(),
                  trs_frame.scale());
}

void FileWriter::flush()
{
    {
        auto duration{gsl::narrow<uint64_t>(last_timecode_ / MATROSKA_TIMESCALE_NS)};

        auto& segment_info{GetChild<KaxInfo>(*segment_)};
        GetChild<KaxDuration>(segment_info).SetValue(duration);
        segment_info_placeholder_->ReplaceWith(segment_info, *io_callback_);
    }

    //
    // render KaxCues
    //
    auto& cues{GetChild<KaxCues>(*segment_)};
    cues.Render(*io_callback_);

    //
    // update KaxSeekHead
    //
    {
        auto& seek_head{GetChild<KaxSeekHead>(*segment_)};

        auto& segment_info{GetChild<KaxInfo>(*segment_)};
        seek_head.IndexThis(segment_info, *segment_);

        auto& tracks{GetChild<KaxTracks>(*segment_)};
        seek_head.IndexThis(tracks, *segment_);

        auto& attachments{GetChild<KaxAttachments>(*segment_)};
        seek_head.IndexThis(attachments, *segment_);

        seek_head.IndexThis(cues, *segment_);

        seek_head_placeholder_->ReplaceWith(seek_head, *io_callback_);
    }

    io_callback_->setFilePointer(0, libebml::seek_end);
    uint64_t segment_size{io_callback_->getFilePointer() - segment_->GetElementPosition() -
                          segment_->HeadSize()};
    segment_->SetSizeInfinite(true);
    if (!segment_->ForceSize(segment_size))
        spdlog::info("Failed to set segment size");
    segment_->OverwriteHead(*io_callback_);
    io_callback_->close();

}

Bytes FileWriter::getBytes()
{
    auto mem_io_callback{reinterpret_cast<MemIOCallback*>(io_callback_.get())};

    // bool ok{mem_io_callback->IsOk()};
    // string error_str{mem_io_callback->GetLastErrorStr()};
    // spdlog::info("ok: {}, error_str: {}", ok, error_str);

    uint64_t size{mem_io_callback->GetDataBufferSize()};
    Bytes bytes(size);
    memcpy(bytes.data(), mem_io_callback->GetDataBuffer(), size);
    return bytes;
}
} // namespace rgbd
