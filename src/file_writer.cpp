#include "file_writer.hpp"

#include <rgbd/png_utils.hpp>
#include <rgbd/rvl_encoder.hpp>

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

unique_ptr<DepthEncoder> create_depth_encoder(DepthCodecType depth_codec_type,
                                              int width,
                                              int height,
                                              int depth_diff_multiplier)
{
    if (depth_codec_type == DepthCodecType::RVL)
        return unique_ptr<DepthEncoder>{new RVLEncoder(width, height)};
    if (depth_codec_type == DepthCodecType::TDC1)
        return unique_ptr<DepthEncoder>{new TDC1Encoder{width, height, depth_diff_multiplier}};

    spdlog::error("Invalid DepthCodecType found: {}", depth_codec_type);
    throw std::runtime_error("Invalid DepthCodecType found");
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

vector<byte> convert_vec3_to_bytes(const glm::vec3 v)
{
    vector<byte> bytes;
    append_bytes(bytes, convert_to_bytes(v.x));
    append_bytes(bytes, convert_to_bytes(v.y));
    append_bytes(bytes, convert_to_bytes(v.z));
    return bytes;
}

FileWriter::FileWriter(const string& file_path,
                       bool has_depth_confidence,
                       const CameraCalibration& calibration,
                       int color_bitrate,
                       int framerate,
                       DepthCodecType depth_codec_type,
                       int depth_diff_multiplier,
                       int samplerate)
    : generator_{get_random_number()}
    , distribution_{std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()}
    , io_callback_{std::make_unique<StdIOCallback>(file_path.c_str(), MODE_CREATE)}
    , segment_{std::make_unique<KaxSegment>()}
    , color_track_{nullptr}
    , depth_track_{nullptr}
    , depth_confidence_track_{nullptr}
    , audio_track_{nullptr}
    , floor_track_{nullptr}
    , acceleration_track_{nullptr}
    , rotation_rate_track_{nullptr}
    , magnetic_field_track_{nullptr}
    , gravity_track_{nullptr}
    , seek_head_placeholder_{nullptr}
    , segment_info_placeholder_{nullptr}
    , initial_time_point_ns_{nullopt}
    , last_timecode_{0}
    , rgbd_index_{0}
    , color_encoder_{ColorCodecType::VP8,
                     calibration.getColorWidth(),
                     calibration.getColorHeight(),
                     color_bitrate,
                     framerate}
    , depth_encoder_{create_depth_encoder(depth_codec_type,
                                          calibration.getDepthWidth(),
                                          calibration.getDepthHeight(),
                                          depth_diff_multiplier)}
    , audio_encoder_{}
    , framerate_{framerate}
    , samplerate_{samplerate}
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
    constexpr uint64_t DEPTH_CONFIDENCE_TRACK_NUMBER{3};
    constexpr uint64_t AUDIO_TRACK_NUMBER{4};
    constexpr uint64_t FLOOR_TRACK_NUMBER{5};
    constexpr uint64_t ACCELERATION_TRACK_NUMBER{6};
    constexpr uint64_t ROTATION_RATE_TRACK_NUMBER{7};
    constexpr uint64_t MAGNETIC_FIELD_TRACK_NUMBER{8};
    constexpr uint64_t GRAVITY_TRACK_NUMBER{9};
    //
    // init color_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        color_track_ = new KaxTrackEntry;
        tracks.PushElement(*color_track_); // Track will be freed when the file is closed.
        color_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        // Track numbers start at 1
        GetChild<KaxTrackNumber>(*color_track_).SetValue(COLOR_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*color_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*color_track_).SetValue(track_video);
        GetChild<KaxTrackName>(*color_track_).SetValueUTF8("COLOR");
        GetChild<KaxCodecID>(*color_track_).SetValue("V_VP8");

        GetChild<KaxTrackDefaultDuration>(*color_track_).SetValue(ONE_SECOND_NS / framerate);
        auto& color_video_track{GetChild<KaxTrackVideo>(*color_track_)};
        GetChild<KaxVideoPixelWidth>(color_video_track).SetValue(calibration.getColorWidth());
        GetChild<KaxVideoPixelHeight>(color_video_track).SetValue(calibration.getColorHeight());
    }
    //
    // depth_track_ init
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        depth_track_ = new KaxTrackEntry;
        tracks.PushElement(*depth_track_); // Track will be freed when the file is closed.
        depth_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*depth_track_).SetValue(DEPTH_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*depth_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*depth_track_).SetValue(track_video);
        GetChild<KaxTrackName>(*depth_track_).SetValueUTF8("DEPTH");
        if (depth_encoder_->getCodecType() == DepthCodecType::RVL) {
            GetChild<KaxCodecID>(*depth_track_).SetValue("V_RVL");
        } else if (depth_encoder_->getCodecType() == DepthCodecType::TDC1) {
            GetChild<KaxCodecID>(*depth_track_).SetValue("V_TDC1");
        } else {
            throw std::runtime_error("Invalid depth codec found");
        }

        GetChild<KaxTrackDefaultDuration>(*depth_track_).SetValue(ONE_SECOND_NS / framerate);
        auto& depth_video_track{GetChild<KaxTrackVideo>(*depth_track_)};
        GetChild<KaxVideoPixelWidth>(depth_video_track).SetValue(calibration.getDepthWidth());
        GetChild<KaxVideoPixelHeight>(depth_video_track).SetValue(calibration.getDepthHeight());
    }
    //
    // depth_confidence_track_ init
    //
    if (has_depth_confidence) {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        depth_confidence_track_ = new KaxTrackEntry;
        tracks.PushElement(
            *depth_confidence_track_); // Track will be freed when the file is closed.
        depth_confidence_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*depth_confidence_track_).SetValue(DEPTH_CONFIDENCE_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*depth_confidence_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*depth_confidence_track_).SetValue(track_video);
        GetChild<KaxTrackName>(*depth_confidence_track_).SetValueUTF8("DEPTH_CONFIDENCE");
        GetChild<KaxCodecID>(*depth_confidence_track_).SetValue("V_RVL");

        GetChild<KaxTrackDefaultDuration>(*depth_confidence_track_)
            .SetValue(ONE_SECOND_NS / framerate);
        auto& depth_confidence_video_track{GetChild<KaxTrackVideo>(*depth_confidence_track_)};
        GetChild<KaxVideoPixelWidth>(depth_confidence_video_track)
            .SetValue(calibration.getDepthWidth());
        GetChild<KaxVideoPixelHeight>(depth_confidence_video_track)
            .SetValue(calibration.getDepthHeight());
    }
    //
    // init audio_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        audio_track_ = new KaxTrackEntry;
        tracks.PushElement(*audio_track_); // Track will be freed when the file is closed.
        audio_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*audio_track_).SetValue(AUDIO_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*audio_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*audio_track_).SetValue(track_audio);
        GetChild<KaxTrackName>(*audio_track_).SetValueUTF8("AUDIO");
        GetChild<KaxCodecID>(*audio_track_).SetValue("A_OPUS");

        // GetChild<KaxCodecDelay>(*audio_track).SetValue(6500000); // from a file from ffmpeg-based
        // recorder
        constexpr uint64_t OPUS_SEEK_PREROLL{80000000}; // from ffmpeg matroskaenc.c
        GetChild<KaxSeekPreRoll>(*audio_track_).SetValue(OPUS_SEEK_PREROLL);

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
        uint32_t intput_samplerate{gsl::narrow<uint32_t>(samplerate)};
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
        GetChild<KaxCodecPrivate>(*audio_track_).SetBuffer(opus_head, OPUS_HEAD_SIZE);

        // KaxTrackDefaultDuration expects "number of nanoseconds (not scaled) per frame" here.
        GetChild<KaxTrackDefaultDuration>(*audio_track_).SetValue(ONE_SECOND_NS / samplerate);
        auto& audio_track_details = GetChild<KaxTrackAudio>(*audio_track_);
        GetChild<KaxAudioSamplingFreq>(audio_track_details).SetValue(samplerate);
        GetChild<KaxAudioOutputSamplingFreq>(audio_track_details).SetValue(samplerate);
        GetChild<KaxAudioChannels>(audio_track_details).SetValue(AUDIO_INPUT_CHANNEL_COUNT);
        GetChild<KaxAudioBitDepth>(audio_track_details).SetValue(sizeof(float) * 8);
    }
    //
    // init floor_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        floor_track_ = new KaxTrackEntry;
        tracks.PushElement(*floor_track_); // Track will be freed when the file is closed.
        floor_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*floor_track_).SetValue(FLOOR_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*floor_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*floor_track_).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*floor_track_).SetValueUTF8("FLOOR");
        GetChild<KaxCodecID>(*floor_track_).SetValue("S_FLOOR");
    }
    //
    // init acceleration_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        acceleration_track_ = new KaxTrackEntry;
        tracks.PushElement(*acceleration_track_); // Track will be freed when the file is closed.
        acceleration_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*acceleration_track_).SetValue(ACCELERATION_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*acceleration_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*acceleration_track_).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*acceleration_track_).SetValueUTF8("ACCELERATION");
        GetChild<KaxCodecID>(*acceleration_track_).SetValue("S_ACCELERATION");
    }
    //
    // init rotation_rate_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        rotation_rate_track_ = new KaxTrackEntry;
        tracks.PushElement(*rotation_rate_track_); // Track will be freed when the file is closed.
        rotation_rate_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*rotation_rate_track_).SetValue(ROTATION_RATE_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*rotation_rate_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*rotation_rate_track_).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*rotation_rate_track_).SetValueUTF8("ROTATION_RATE");
        GetChild<KaxCodecID>(*rotation_rate_track_).SetValue("S_ROTATION_RATE");
    }
    //
    // init magnetic_field_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        magnetic_field_track_ = new KaxTrackEntry;
        tracks.PushElement(*magnetic_field_track_); // Track will be freed when the file is closed.
        magnetic_field_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*magnetic_field_track_).SetValue(MAGNETIC_FIELD_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*magnetic_field_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*magnetic_field_track_).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*magnetic_field_track_).SetValueUTF8("MAGNETIC_FIELD");
        GetChild<KaxCodecID>(*magnetic_field_track_).SetValue("S_MAGNETIC_FIELD");
    }
    //
    // init gravity_track_
    //
    {
        auto& tracks{GetChild<KaxTracks>(*segment_)};
        gravity_track_ = new KaxTrackEntry;
        tracks.PushElement(*gravity_track_); // Track will be freed when the file is closed.
        gravity_track_->SetGlobalTimecodeScale(MATROSKA_TIMESCALE_NS);

        GetChild<KaxTrackNumber>(*gravity_track_).SetValue(GRAVITY_TRACK_NUMBER);
        GetChild<KaxTrackUID>(*gravity_track_).SetValue(distribution_(generator_));
        GetChild<KaxTrackType>(*gravity_track_).SetValue(track_subtitle);
        GetChild<KaxTrackName>(*gravity_track_).SetValueUTF8("GRAVITY");
        GetChild<KaxCodecID>(*gravity_track_).SetValue("S_GRAVITY");
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
        seek_head_placeholder_->SetSize(1024);
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

void FileWriter::writeVideoFrame(const Frame& frame)
{
    writeVideoFrame(frame.time_point_us(),
                    frame.yuv_frame(),
                    frame.depth_frame(),
                    frame.depth_confidence_frame(),
                    frame.floor());
}

void FileWriter::writeVideoFrame(int64_t time_point_us,
                                 const YuvFrame& yuv_frame,
                                 const Int32Frame& depth_frame,
                                 const optional<UInt8Frame>& depth_confidence_frame,
                                 const Plane& floor)
{
    if (depth_confidence_frame) {
        writeVideoFrame(time_point_us,
                        yuv_frame.width(),
                        yuv_frame.height(),
                        yuv_frame.y_channel(),
                        yuv_frame.u_channel(),
                        yuv_frame.v_channel(),
                        depth_frame.values(),
                        depth_confidence_frame->values(),
                        floor);
    } else {
        writeVideoFrame(time_point_us,
                        yuv_frame.width(),
                        yuv_frame.height(),
                        yuv_frame.y_channel(),
                        yuv_frame.u_channel(),
                        yuv_frame.v_channel(),
                        depth_frame.values(),
                        nullopt,
                        floor);
    }
}

void FileWriter::writeVideoFrame(int64_t time_point_us,
                                 int width,
                                 int height,
                                 gsl::span<const uint8_t> y_channel,
                                 gsl::span<const uint8_t> u_channel,
                                 gsl::span<const uint8_t> v_channel,
                                 gsl::span<const int32_t> depth_values,
                                 optional<gsl::span<const uint8_t>> depth_confidence_values,
                                 const Plane& floor)
{
    if (depth_confidence_track_ && !depth_confidence_values)
        throw std::runtime_error("Video has depth confidence track but not found in frame.");

    if (!depth_confidence_track_ && depth_confidence_values)
        throw std::runtime_error("Video has no depth confidence track but found in frame.");

    int64_t time_point_ns{time_point_us * 1000};
    if (!initial_time_point_ns_)
        initial_time_point_ns_ = time_point_ns;

    auto& cues{GetChild<KaxCues>(*segment_)};
    auto video_timecode{gsl::narrow<uint64_t>(time_point_ns - *initial_time_point_ns_)};

    // A keyframe every two seconds.
    bool keyframe{rgbd_index_ % (framerate_ * 2) == 0};

    auto video_cluster{new KaxCluster};
    segment_->PushElement(*video_cluster);
    video_cluster->InitTimecode(video_timecode / MATROSKA_TIMESCALE_NS, MATROSKA_TIMESCALE_NS);
    video_cluster->SetParent(*segment_);
    video_cluster->EnableChecksum();

    auto color_bytes{
        color_encoder_.encode(y_channel, u_channel, v_channel, keyframe).getDataBytes()};
    auto color_block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
    auto color_data_buffer{new DataBuffer{reinterpret_cast<uint8_t*>(color_bytes.data()),
                                          gsl::narrow<uint32_t>(color_bytes.size())}};
    video_cluster->AddBlockBlob(color_block_blob);
    color_block_blob->SetParent(*video_cluster);
    color_block_blob->AddFrameAuto(*color_track_, video_timecode, *color_data_buffer);

    auto depth_bytes{depth_encoder_->encode(depth_values, keyframe)};
    auto depth_block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
    auto depth_data_buffer{new DataBuffer{reinterpret_cast<uint8_t*>(depth_bytes.data()),
                                          gsl::narrow<uint32_t>(depth_bytes.size())}};
    video_cluster->AddBlockBlob(depth_block_blob);
    depth_block_blob->SetParent(*video_cluster);
    depth_block_blob->AddFrameAuto(*depth_track_, video_timecode, *depth_data_buffer);

    // depth_confidence_bytes needs to stay outside to keep the bytes inside it alive
    // until the video_cluster is Render()ed.
    Bytes depth_confidence_bytes;
    if (depth_confidence_values) {
        depth_confidence_bytes = rvl::compress<uint8_t>(*depth_confidence_values);
        auto depth_confidence_block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
        auto depth_confidence_data_buffer{
            new DataBuffer{reinterpret_cast<uint8_t*>(depth_confidence_bytes.data()),
                           gsl::narrow<uint32_t>(depth_confidence_bytes.size())}};
        video_cluster->AddBlockBlob(depth_confidence_block_blob);
        depth_confidence_block_blob->SetParent(*video_cluster);
        depth_confidence_block_blob->AddFrameAuto(
            *depth_confidence_track_, video_timecode, *depth_confidence_data_buffer);
    }

    auto floor_bytes{floor.toBytes()};
    auto floor_block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
    auto floor_data_buffer{new DataBuffer{reinterpret_cast<uint8_t*>(floor_bytes.data()),
                                          gsl::narrow<uint32_t>(floor_bytes.size())}};
    video_cluster->AddBlockBlob(floor_block_blob);
    floor_block_blob->SetParent(*video_cluster);
    floor_block_blob->AddFrameAuto(*floor_track_, video_timecode, *floor_data_buffer);

    video_cluster->Render(*io_callback_, cues);
    video_cluster->ReleaseFrames();

    last_timecode_ = video_timecode;
    ++rgbd_index_;
}

void FileWriter::writeAudioFrame(const AudioFrame& audio_frame)
{
    writeAudioFrame(audio_frame.time_point_us(), audio_frame.pcm_samples());
}

void FileWriter::writeAudioFrame(int64_t time_point_us, gsl::span<const float> pcm_samples)
{
    if (pcm_samples.size() % AUDIO_INPUT_SAMPLES_PER_FRAME != 0)
        throw std::runtime_error("pcm_samples.size() % AUDIO_INPUT_SAMPLES_PER_FRAME != 0");

    int64_t time_point_ns{time_point_us * 1000};
    if (!initial_time_point_ns_)
        initial_time_point_ns_ = time_point_ns;

    auto audio_frame_timecode{gsl::narrow<uint64_t>(time_point_ns - *initial_time_point_ns_)};

    vector<AVPacketHandle> audio_packets;
    for (int i{0}; i < pcm_samples.size(); i += AUDIO_INPUT_SAMPLES_PER_FRAME)
        audio_encoder_.encode({&pcm_samples[i], AUDIO_INPUT_SAMPLES_PER_FRAME}, audio_packets);

    auto& cues{GetChild<KaxCues>(*segment_)};

    const auto CLUSTER_TIMECODE_INTERVAL{
        gsl::narrow<uint64_t>(AUDIO_INPUT_SAMPLES_PER_FRAME * ONE_SECOND_NS / samplerate_)};
    for (int i{0}; i < audio_packets.size(); ++i) {
        auto& audio_packet{audio_packets[i]};
        auto audio_cluster_timecode{audio_frame_timecode + i * CLUSTER_TIMECODE_INTERVAL};

        auto audio_bytes{audio_packet.getDataBytes()};

        auto audio_cluster{new KaxCluster};
        segment_->PushElement(*audio_cluster);
        audio_cluster->InitTimecode(audio_cluster_timecode / MATROSKA_TIMESCALE_NS,
                                    MATROSKA_TIMESCALE_NS);
        audio_cluster->SetParent(*segment_);
        audio_cluster->EnableChecksum();

        auto block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
        auto data_buffer{new DataBuffer{reinterpret_cast<uint8_t*>(audio_bytes.data()),
                                        gsl::narrow<uint32_t>(audio_bytes.size())}};
        audio_cluster->AddBlockBlob(block_blob);
        block_blob->SetParent(*audio_cluster);
        block_blob->AddFrameAuto(*audio_track_, audio_cluster_timecode, *data_buffer);

        audio_cluster->Render(*io_callback_, cues);
        audio_cluster->ReleaseFrames();

        last_timecode_ = audio_cluster_timecode;
    }
}

void FileWriter::writeImuFrame(int64_t time_point_us,
                               glm::vec3 acceleration,
                               glm::vec3 rotation_rate,
                               glm::vec3 magnetic_field,
                               glm::vec3 gravity)
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

    auto acceleration_block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
    auto acceleration_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(acceleration_bytes.data()),
                       gsl::narrow<uint32_t>(acceleration_bytes.size())}};
    imu_cluster->AddBlockBlob(acceleration_block_blob);
    acceleration_block_blob->SetParent(*imu_cluster);
    acceleration_block_blob->AddFrameAuto(*acceleration_track_, imu_timecode, *acceleration_data_buffer);

    vector<byte> rotation_rate_bytes(convert_vec3_to_bytes(rotation_rate));

    auto rotation_rate_block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
    auto rotation_rate_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(rotation_rate_bytes.data()),
                       gsl::narrow<uint32_t>(rotation_rate_bytes.size())}};
    imu_cluster->AddBlockBlob(rotation_rate_block_blob);
    rotation_rate_block_blob->SetParent(*imu_cluster);
    rotation_rate_block_blob->AddFrameAuto(*rotation_rate_track_, imu_timecode, *rotation_rate_data_buffer);

    vector<byte> magnetic_field_bytes(convert_vec3_to_bytes(magnetic_field));

    auto magnetic_field_block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
    auto magnetic_field_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(magnetic_field_bytes.data()),
                       gsl::narrow<uint32_t>(magnetic_field_bytes.size())}};
    imu_cluster->AddBlockBlob(magnetic_field_block_blob);
    magnetic_field_block_blob->SetParent(*imu_cluster);
    magnetic_field_block_blob->AddFrameAuto(*magnetic_field_track_, imu_timecode, *magnetic_field_data_buffer);

    vector<byte> gravity_bytes(convert_vec3_to_bytes(gravity));

    auto gravity_block_blob{new KaxBlockBlob(BLOCK_BLOB_SIMPLE_AUTO)};
    auto gravity_data_buffer{
        new DataBuffer{reinterpret_cast<uint8_t*>(gravity_bytes.data()),
                       gsl::narrow<uint32_t>(gravity_bytes.size())}};
    imu_cluster->AddBlockBlob(gravity_block_blob);
    gravity_block_blob->SetParent(*imu_cluster);
    gravity_block_blob->AddFrameAuto(*gravity_track_, imu_timecode, *gravity_data_buffer);

    imu_cluster->Render(*io_callback_, cues);
    imu_cluster->ReleaseFrames();

    last_timecode_ = imu_timecode;
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
} // namespace rgbd