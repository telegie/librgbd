#include "file_parser.hpp"

#include <rgbd/ios_camera_calibration.hpp>
#include <rgbd/kinect_camera_calibration.hpp>
#include <rgbd/undistorted_camera_calibration.hpp>
#include <rgbd/tdc1_decoder.hpp>

using namespace libmatroska;

namespace rgbd
{
std::unique_ptr<EbmlElement> next_child(IOCallback& input, EbmlStream& stream, EbmlElement* parent)
{
    try {
        int upper_level = 0;
        EbmlElement* element = stream.FindNextElement(
            parent->Generic().Context, upper_level, parent->GetSize(), false, 0);

        // upper_level shows the relationship of the element to the parent element
        // -1 : global element
        //  0 : child
        //  1 : same level
        //  + : further parent
        if (upper_level > 0) {
            // This element is not a child of the parent, set the file pointer back to the start of
            // the element and return nullptr.
            uint64_t file_offset = element->GetElementPosition();
            assert(file_offset <= INT64_MAX);
            input.setFilePointer((int64_t)file_offset);
            delete element;
            return nullptr;
        }

        return std::unique_ptr<EbmlElement>(element);
    } catch (std::ios_base::failure& e) {
        spdlog::error("Failed to get next child (parent id %x) in recording: %s",
                      EbmlId(*parent).GetValue(),
                      e.what());
        return nullptr;
    }
}

/**
 * Find the next element of type T at the current file offset.
 * If \p search is true, this function will keep reading elements until an element of type T is
 * found or EOF is reached. If \p search is false, this function will only return an element if it
 * exists at the current file offset.
 *
 * Example usage: find_next<KaxSegment>(context, true);
 */
template <typename T> std::unique_ptr<T> find_next(EbmlStream& stream, bool search = false)
{
    try {
        EbmlElement* element = nullptr;
        do {
            if (element) {
                if (!element->IsFiniteSize()) {
                    spdlog::error("Failed to read recording: Element Id '{}' has unknown size",
                                  EbmlId(*element).GetValue());
                    delete element;
                    return nullptr;
                }
                element->SkipData(stream, element->Generic().Context);
                delete element;
                element = nullptr;
            }
            if (!element) {
                element = stream.FindNextID(T::ClassInfos, UINT64_MAX);
            }
            if (!search) {
                break;
            }
        } while (element && EbmlId(*element) != T::ClassInfos.GlobalId);

        if (!element) {
            if (!search) {
                spdlog::error("Failed to read recording: Element Id '{}' not found",
                              T::ClassInfos.GlobalId.GetValue());
            }
            return nullptr;
        } else if (EbmlId(*element) != T::ClassInfos.GlobalId) {
            spdlog::error("Failed to read recording: Expected element {} (id {}), found id '{}'",
                          T::ClassInfos.GetName(),
                          T::ClassInfos.GlobalId.GetValue(),
                          EbmlId(*element).GetValue());
            delete element;
            return nullptr;
        }

        return std::unique_ptr<T>(static_cast<T*>(element));
    } catch (std::ios_base::failure& e) {
        spdlog::error("Failed to find {} in recording: %s", T::ClassInfos.GetName(), e.what());
        return nullptr;
    }
}

// Template helper functions
template <typename T> T* read_element(EbmlStream& stream, EbmlElement* element)
{
    try {
        int upper_level = 0;
        EbmlElement* dummy = nullptr;

        T* typed_element = static_cast<T*>(element);
        typed_element->Read(stream, T::ClassInfos.Context, upper_level, dummy, true);
        return typed_element;
    } catch (std::ios_base::failure& e) {
        spdlog::error(
            "Failed to read element {} in recording: {}", T::ClassInfos.GetName(), e.what());
        return nullptr;
    }
}

template <typename T>
std::unique_ptr<T>
read_offset(IOCallback& input, EbmlStream& stream, KaxSegment& segment, uint64_t offset)
{
    input.setFilePointer(gsl::narrow<int64_t>(segment.GetGlobalPosition(offset)));
    auto element_out = find_next<T>(stream);
    if (element_out) {
        if (read_element<T>(stream, element_out.get()) == NULL) {
            throw std::runtime_error(
                fmt::format("Failed to read element: {} at offset {}", typeid(T).name(), offset));
        }
        return element_out;
    } else {
        throw std::runtime_error(
            fmt::format("Element not found at offset: {} at offset {}", typeid(T).name(), offset));
    }
}

Bytes copy_data_buffer_to_bytes(DataBuffer& data_buffer)
{
    Bytes bytes(data_buffer.Size());
    memcpy(bytes.data(), data_buffer.Buffer(), data_buffer.Size());
    return bytes;
}

glm::vec3 read_vec3(const vector<byte>& bytes)
{
    int cursor{0};
    float x{read_from_bytes<float>(bytes, cursor)};
    float y{read_from_bytes<float>(bytes, cursor)};
    float z{read_from_bytes<float>(bytes, cursor)};
    return glm::vec3{x, y, z};
}

glm::quat read_quat(const vector<byte>& bytes)
{
    int cursor{0};
    float w{read_from_bytes<float>(bytes, cursor)};
    float x{read_from_bytes<float>(bytes, cursor)};
    float y{read_from_bytes<float>(bytes, cursor)};
    float z{read_from_bytes<float>(bytes, cursor)};
    return glm::quat{w, x, y, z};
}

FileParser::FileParser(const void* ptr, size_t size)
    : input_{new MemReadIOCallback{ptr, size}}
    , stream_{*input_}
    , kax_segment_{}
    , file_offsets_{}
    , file_info_{}
    , file_tracks_{}
    , file_attachments_{}
{
    parseExceptClusters();
}

FileParser::FileParser(const string& file_path)
    : input_{new StdIOCallback{file_path.c_str(), open_mode::MODE_READ}}
    , stream_{*input_}
    , kax_segment_{}
    , file_offsets_{}
    , file_info_{}
    , file_tracks_{}
    , file_attachments_{}
{
    parseExceptClusters();
}

void FileParser::parseExceptClusters()
{
    auto head{find_next<EbmlHead>(stream_)};
    if (!head)
        throw std::runtime_error("EBML head missing");

    if (read_element<EbmlHead>(stream_, head.get()) == nullptr)
        throw std::runtime_error("Failed reading head");

//    string doc_type{GetChild<EDocType>(*head).GetValue()};
//    uint64_t doc_type_version{GetChild<EDocTypeVersion>(*head).GetValue()};
//    uint64_t doc_type_read_version{GetChild<EDocTypeReadVersion>(*head).GetValue()};
//
//    spdlog::info("doc_type: {}, doc_type_version: {}, doc_type_read_version: {}",
//                 doc_type,
//                 doc_type_version,
//                 doc_type_read_version);

    kax_segment_ = find_next<KaxSegment>(stream_, true);
    if (!kax_segment_)
        throw std::runtime_error("No segment");

    file_offsets_ = parseOffsets(kax_segment_);

    if (!file_offsets_) {
        spdlog::error("Failed to parse offsets...");
        throw std::runtime_error("Failed to parse offsets...");
    }

    auto kax_info{
        read_offset<KaxInfo>(*input_, stream_, *kax_segment_, file_offsets_->segment_info_offset)};
    file_info_ = parseInfo(kax_info);

    if (!file_info_) {
        spdlog::error("Failed to parse info...");
        throw std::runtime_error("Failed to parse info...");
    }

    auto kax_tracks{
        read_offset<KaxTracks>(*input_, stream_, *kax_segment_, file_offsets_->tracks_offset)};
    file_tracks_ = parseTracks(kax_tracks);

    if (!file_tracks_) {
        spdlog::error("Failed to parse tracks...");
        throw std::runtime_error("Failed to parse tracks...");
    }

    auto attachments{read_offset<KaxAttachments>(
        *input_, stream_, *kax_segment_, file_offsets_->attachments_offset)};

    file_attachments_ = parseAttachments(attachments);
}

optional<const FileInfo> FileParser::parseInfo(unique_ptr<libmatroska::KaxInfo>& kax_info)
{
    auto kax_timecode_scale{FindChild<KaxTimecodeScale>(*kax_info)};
    if (!kax_timecode_scale) {
        spdlog::error("No KaxTimecodeScale");
        return nullopt;
    }
    auto kax_duration{FindChild<KaxDuration>(*kax_info)};
    if (!kax_duration) {
        spdlog::error("No KaxDuration");
        return nullopt;
    }
    auto kax_writing_app{FindChild<KaxWritingApp>(*kax_info)};
    if (!kax_writing_app) {
        spdlog::error("No KaxWritingApp");
        return nullopt;
    }

    FileInfo file_info;
    file_info.timecode_scale_ns = kax_timecode_scale->GetValue();
    file_info.duration_us = kax_duration->GetValue();
    file_info.writing_app = kax_writing_app->GetValue().GetUTF8();
    return file_info;
}

optional<const FileOffsets> FileParser::parseOffsets(unique_ptr<KaxSegment>& segment)
{
    optional<int64_t> segment_info_offset{nullopt};
    optional<int64_t> tracks_offset{nullopt};
    optional<int64_t> attachments_offset{nullopt};
    optional<int64_t> first_cluster_offset{nullopt};

    auto element{next_child(*input_, stream_, kax_segment_.get())};
    while (element != nullptr) {
        if (segment_info_offset && tracks_offset && attachments_offset && first_cluster_offset) {
            FileOffsets offsets;
            offsets.segment_info_offset = *segment_info_offset;
            offsets.tracks_offset = *tracks_offset;
            offsets.attachments_offset = *attachments_offset;
            offsets.first_cluster_offset = *first_cluster_offset;

            return offsets;
        }

        EbmlId element_id(*element);
        if (element_id == KaxSeekHead::ClassInfos.GlobalId) {
            // Parse SeekHead offset positions
            KaxSeekHead* seek_head{read_element<KaxSeekHead>(stream_, element.get())};
            for (EbmlElement* e : seek_head->GetElementList()) {
                if (EbmlId(*e) == KaxSeek::ClassInfos.GlobalId) {
                    auto seek{read_element<KaxSeek>(stream_, e)};
                    KaxSeekID& seek_id{GetChild<KaxSeekID>(*seek)};
                    EbmlId ebml_id(seek_id.GetBuffer(),
                                   static_cast<unsigned int>(seek_id.GetSize()));
                    int64_t seek_location{seek->Location()};

                    if (ebml_id == KaxInfo::ClassInfos.GlobalId) {
                        segment_info_offset = seek_location;
                    } else if (ebml_id == KaxTracks::ClassInfos.GlobalId) {
                        tracks_offset = seek_location;
                    } else if (ebml_id == KaxAttachments::ClassInfos.GlobalId) {
                        attachments_offset = seek_location;
                    } else if (ebml_id == KaxCues::ClassInfos.GlobalId) {
                        // Not using Cues for now.
//                        spdlog::info("Found cues");
                    } else {
                        spdlog::info("Found seek not used.");
                    }
                }
            }
        } else if (element_id == KaxCluster::ClassInfos.GlobalId) {
            if (!first_cluster_offset) {
                first_cluster_offset = segment->GetRelativePosition(*element.get());
            }
        } else {
            element->SkipData(stream_, element->Generic().Context);
        }

        element = next_child(*input_, stream_, segment.get());
    }

    return nullopt;
}

optional<const FileTracks> FileParser::parseTracks(unique_ptr<KaxTracks>& tracks)
{
    optional<FileVideoTrack> color_track{nullopt};
    optional<FileDepthVideoTrack> depth_track{nullopt};
    optional<FileAudioTrack> audio_track{nullopt};
    optional<int> floor_track_number{nullopt};
    optional<int> acceleration_track_number{nullopt};
    optional<int> rotation_rate_track_number{nullopt};
    optional<int> magnetic_field_track_number{nullopt};
    optional<int> gravity_track_number{nullopt};
    optional<int> translation_track_number{nullopt};
    optional<int> rotation_track_number{nullopt};
    optional<int> scale_track_number{nullopt};

    for (EbmlElement* e : tracks->GetElementList()) {
        if (EbmlId(*e) == KaxTrackEntry::ClassInfos.GlobalId) {
            KaxTrackEntry* track_entry{read_element<KaxTrackEntry>(stream_, e)};
            if (!track_entry)
                throw std::runtime_error("Failed reading track_entry");

            uint64_t track_number{GetChild<KaxTrackNumber>(*track_entry).GetValue()};
            string track_name{GetChild<KaxTrackName>(*track_entry).GetValueUTF8()};
            string codec_id{GetChild<KaxCodecID>(*track_entry).GetValue()};

            if (track_name == "COLOR") {
                auto& track_video{GetChild<KaxTrackVideo>(*track_entry)};
                uint64_t default_duration{
                    GetChild<KaxTrackDefaultDuration>(track_video).GetValue()};
                uint64_t width{GetChild<KaxVideoPixelWidth>(track_video).GetValue()};
                uint64_t height{GetChild<KaxVideoPixelHeight>(track_video).GetValue()};

                color_track = FileVideoTrack{};
                color_track->track_number = gsl::narrow<int>(track_number);
                color_track->codec = codec_id;
                color_track->default_duration_ns = default_duration;
                color_track->width = gsl::narrow<int>(width);
                color_track->height = gsl::narrow<int>(height);
            } else if (track_name == "DEPTH") {
                auto& track_video{GetChild<KaxTrackVideo>(*track_entry)};
                uint64_t default_duration{
                    GetChild<KaxTrackDefaultDuration>(track_video).GetValue()};
                uint64_t width{GetChild<KaxVideoPixelWidth>(track_video).GetValue()};
                uint64_t height{GetChild<KaxVideoPixelHeight>(track_video).GetValue()};

                float depth_unit{DEFAULT_DEPTH_UNIT};
                auto codec_private{FindChild<KaxCodecPrivate>(*track_entry)};
                if (codec_private) {
                    vector<char> codec_private_vector(codec_private->GetSize());
                    memcpy(codec_private_vector.data(), codec_private->GetBuffer(), codec_private->GetSize());
                    string codec_private_str{codec_private_vector.begin(), codec_private_vector.end()};
                    // Brace initialization of json behaves differently in gcc than in clang.
                    // Do not use brace initialization.
                    // reference: https://github.com/nlohmann/json/issues/2339
                    json codec_private_json(json::parse(codec_private_str));
                    depth_unit = codec_private_json["depthUnit"].get<float>();
                }

                depth_track = FileDepthVideoTrack{};
                depth_track->track_number = gsl::narrow<int>(track_number);
                depth_track->codec = codec_id;
                depth_track->default_duration_ns = default_duration;
                depth_track->width = gsl::narrow<int>(width);
                depth_track->height = gsl::narrow<int>(height);
                depth_track->depth_unit = depth_unit;
            } else if (track_name == "AUDIO") {
                auto& track_audio{GetChild<KaxTrackAudio>(*track_entry)};
                double sampling_freq{GetChild<KaxAudioSamplingFreq>(track_audio).GetValue()};

                audio_track = FileAudioTrack{};
                audio_track->track_number = gsl::narrow<int>(track_number);
                audio_track->sampling_frequency = sampling_freq;
            } else if (track_name == "FLOOR") {
                floor_track_number = gsl::narrow<int>(track_number);
            } else if (track_name == "ACCELERATION") {
                acceleration_track_number = gsl::narrow<int>(track_number);
            } else if (track_name == "ROTATION_RATE") {
                rotation_rate_track_number = gsl::narrow<int>(track_number);
            } else if (track_name == "MAGNETIC_FIELD") {
                magnetic_field_track_number = gsl::narrow<int>(track_number);
            } else if (track_name == "GRAVITY") {
                gravity_track_number = gsl::narrow<int>(track_number);
            } else if (track_name == "TRANSLATION") {
                translation_track_number = gsl::narrow<int>(track_number);
            } else if (track_name == "ROTATION") {
                rotation_track_number = gsl::narrow<int>(track_number);
            } else if (track_name == "SCALE") {
                scale_track_number = gsl::narrow<int>(track_number);
            } else {
                spdlog::error("Invalid track_name: {}", track_name);
            }
        } else {
            spdlog::info("is not KaxTrackEntry");
        }
    }

    if (!color_track || !depth_track || !audio_track)
        return nullopt;

    FileTracks file_tracks;
    file_tracks.color_track = *color_track;
    file_tracks.depth_track = *depth_track;
    file_tracks.audio_track = *audio_track;
    file_tracks.floor_track_number = floor_track_number;
    file_tracks.acceleration_track_number = acceleration_track_number;
    file_tracks.rotation_rate_track_number = rotation_rate_track_number;
    file_tracks.magnetic_field_track_number = magnetic_field_track_number;
    file_tracks.gravity_track_number = gravity_track_number;
    file_tracks.translation_track_number = translation_track_number;
    file_tracks.rotation_track_number = rotation_track_number;
    file_tracks.scale_track_number = scale_track_number;

    return file_tracks;
}

optional<const FileAttachments>
FileParser::parseAttachments(unique_ptr<libmatroska::KaxAttachments>& attachments)
{
    shared_ptr<CameraCalibration> camera_calibration;
    optional<Bytes> cover_png_bytes;

    for (EbmlElement* e : attachments->GetElementList()) {
        if (EbmlId(*e) == KaxAttached::ClassInfos.GlobalId) {
            KaxAttached* attached_file{read_element<KaxAttached>(stream_, e)};
            if (!attached_file)
                throw std::runtime_error("Failed reading attached_file");

            auto file_name{GetChild<KaxFileName>(*attached_file).GetValue().GetUTF8()};
            if (file_name == "calibration.json") {
                auto& file_data{GetChild<KaxFileData>(*attached_file)};
                vector<char> calibration_vector(file_data.GetSize());
                memcpy(calibration_vector.data(), file_data.GetBuffer(), file_data.GetSize());
                string calibration_str{calibration_vector.begin(), calibration_vector.end()};
                // Brace initialization of json behaves differently in gcc than in clang.
                // Do not use brace initialization.
                // reference: https://github.com/nlohmann/json/issues/2339
                json calibration_json(json::parse(calibration_str));

                string calibration_type{calibration_json["calibrationType"].get<string>()};
                if (calibration_type == "azureKinect") {
                    camera_calibration = shared_ptr<CameraCalibration>(new KinectCameraCalibration{
                        KinectCameraCalibration::fromJson(calibration_json)});
                } else if (calibration_type == "ios") {
                    camera_calibration = shared_ptr<CameraCalibration>(
                        new IosCameraCalibration{IosCameraCalibration::fromJson(calibration_json)});
                } else if (calibration_type == "undistorted") {
                    camera_calibration = shared_ptr<CameraCalibration>(
                        new UndistortedCameraCalibration{UndistortedCameraCalibration::fromJson(calibration_json)});
                } else {
                    throw std::runtime_error("Invalid calibration_type");
                }
            } else if (file_name == "cover.png") {
                auto& file_data{GetChild<KaxFileData>(*attached_file)};
                cover_png_bytes = Bytes(file_data.GetSize());
                memcpy(cover_png_bytes->data(), file_data.GetBuffer(), file_data.GetSize());
            } else {
                throw std::runtime_error("Invalid attached file found");
            };
        }
    }

    if (!camera_calibration)
        return nullopt;

    FileAttachments file_attachments;
    file_attachments.camera_calibration = camera_calibration;
    file_attachments.cover_png_bytes = cover_png_bytes;

    return file_attachments;
}

FileFrame* FileParser::parseCluster(unique_ptr<libmatroska::KaxCluster>& cluster)
{
    if (read_element<KaxCluster>(stream_, cluster.get()) == nullptr)
        throw std::runtime_error{"Failed reading cluster"};
    auto cluster_timecode{FindChild<KaxClusterTimecode>(*cluster)->GetValue()};
    cluster->InitTimecode(cluster_timecode / file_info_->timecode_scale_ns,
                          file_info_->timecode_scale_ns);

    int64 global_timecode{0};
    optional<bool> keyframe{nullopt};
    Bytes color_bytes;
    Bytes depth_bytes;
    optional<Plane> floor{nullopt};
    FileAudioFrame* audio_frame{nullptr};
    optional<glm::vec3> acceleration{nullopt};
    optional<glm::vec3> rotation_rate{nullopt};
    optional<glm::vec3> magnetic_field{nullopt};
    optional<glm::vec3> gravity{nullopt};
    optional<glm::vec3> translation{nullopt};
    optional<glm::quat> rotation{nullopt};
    optional<glm::vec3> scale{nullopt};

    for (EbmlElement* e : cluster->GetElementList()) {
        EbmlId id{*e};
        if (id == KaxClusterTimecode::ClassInfos.GlobalId) {
        } else if (id == KaxSimpleBlock::ClassInfos.GlobalId) {
            auto simple_block{static_cast<KaxSimpleBlock*>(e)};
            simple_block->SetParent(*cluster);
            auto track_number{simple_block->TrackNum()};
            auto block_global_timecode{gsl::narrow<int64_t>(simple_block->GlobalTimecode())};
            auto data_buffer{simple_block->GetBuffer(0)};
            if (track_number == file_tracks_->color_track.track_number) {
                global_timecode = block_global_timecode;
                color_bytes = copy_data_buffer_to_bytes(data_buffer);
            } else if (track_number == file_tracks_->depth_track.track_number) {
                depth_bytes = copy_data_buffer_to_bytes(data_buffer);

                keyframe = simple_block->IsKeyframe();
                // The below step is added since before 1.4.0, FileWriter was incorrectly
                // flagging all frames as keyframes.
                // Depth frames were correctly marked whether they were keyframe or not,
                // so using this information to obtain correct information.
                if (file_tracks_->depth_track.codec == "V_TDC1") {
                    if (!is_tdc1_keyframe(depth_bytes)) {
                        keyframe = false;
                    }
                }
            } else if (track_number == file_tracks_->audio_track.track_number) {
                audio_frame = new FileAudioFrame{block_global_timecode,
                                                 copy_data_buffer_to_bytes(data_buffer)};
            } else if (track_number == file_tracks_->floor_track_number) {
                floor = Plane::fromBytes(copy_data_buffer_to_bytes(data_buffer));
            } else if (track_number == file_tracks_->acceleration_track_number) {
                global_timecode = block_global_timecode;
                acceleration = read_vec3(copy_data_buffer_to_bytes(data_buffer));
            } else if (track_number == file_tracks_->rotation_rate_track_number) {
                rotation_rate = read_vec3(copy_data_buffer_to_bytes(data_buffer));
            } else if (track_number == file_tracks_->magnetic_field_track_number) {
                magnetic_field = read_vec3(copy_data_buffer_to_bytes(data_buffer));
            } else if (track_number == file_tracks_->gravity_track_number) {
                gravity = read_vec3(copy_data_buffer_to_bytes(data_buffer));
            } else if (track_number == file_tracks_->translation_track_number) {
                global_timecode = block_global_timecode;
                translation = read_vec3(copy_data_buffer_to_bytes(data_buffer));
            } else if (track_number == file_tracks_->rotation_track_number) {
                rotation = read_quat(copy_data_buffer_to_bytes(data_buffer));
            } else if (track_number == file_tracks_->scale_track_number) {
                scale = read_vec3(copy_data_buffer_to_bytes(data_buffer));
            } else {
                throw std::runtime_error{"Invalid track number from simple_block"};
            }
        } else {
            throw std::runtime_error{"Invalid element from KaxCluster"};
        }
    }

    // emplace only when the cluster is for video, not audio.
    if (color_bytes.size() > 0) {
        if (!keyframe)
            throw std::runtime_error("Failed to find keyframe info.");
        return new FileVideoFrame{
            global_timecode, *keyframe, color_bytes, depth_bytes, floor};
    }

    if (audio_frame) {
        return audio_frame;
    }

    if (acceleration) {
        if (!rotation_rate)
            throw std::runtime_error{"Failed to find rotation_rate"};
        if (!magnetic_field)
            throw std::runtime_error{"Failed to find magnetic_field"};
        if (!gravity)
            throw std::runtime_error{"Failed to find gravity"};

        return new FileIMUFrame{
            global_timecode, *acceleration, *rotation_rate, *magnetic_field, *gravity};
    }

    if (translation) {
        if (!rotation)
            throw std::runtime_error("Failed to find rotation");
        if (!scale)
            throw std::runtime_error("Failed to find scale");

        return new FileTRSFrame{global_timecode, *translation, *rotation, *scale};
    }

    throw std::runtime_error{"No frame from FileParser::parseCluster"};
}

unique_ptr<File> FileParser::parseAllClusters()
{
    auto cluster{read_offset<KaxCluster>(
        *input_, stream_, *kax_segment_, file_offsets_->first_cluster_offset)};

    if (!cluster)
        throw std::runtime_error("Failed to read first cluster");

    vector<unique_ptr<FileVideoFrame>> video_frames;
    vector<unique_ptr<FileAudioFrame>> audio_frames;
    vector<unique_ptr<FileIMUFrame>> imu_frames;
    vector<unique_ptr<FileTRSFrame>> trs_frames;

    while (cluster != nullptr) {
        auto frame{parseCluster(cluster)};
        cluster = find_next<KaxCluster>(stream_, true);
        switch (frame->getType()) {
        case FileFrameType::Video: {
            auto video_frame{dynamic_cast<FileVideoFrame*>(frame)};
            video_frames.emplace_back(video_frame);
            break;
        }
        case FileFrameType::Audio: {
            auto audio_frame{dynamic_cast<FileAudioFrame*>(frame)};
            audio_frames.emplace_back(audio_frame);
            break;
        }
        case FileFrameType::IMU: {
            auto imu_frame{dynamic_cast<FileIMUFrame*>(frame)};
            imu_frames.emplace_back(imu_frame);
            break;
        }
        case FileFrameType::TRS: {
            auto trs_frame{dynamic_cast<FileTRSFrame*>(frame)};
            trs_frames.emplace_back(trs_frame);
            break;
        }
        default:
            throw std::runtime_error{"Invalid FileFrameType found in FileParser::parseAllClusters"};
        }
    }

    return std::make_unique<File>(*file_offsets_,
                                  *file_info_,
                                  *file_tracks_,
                                  *file_attachments_,
                                  std::move(video_frames),
                                  std::move(audio_frames),
                                  std::move(imu_frames),
                                  std::move(trs_frames));
}

unique_ptr<File> FileParser::parseNoFrames()
{
    vector<unique_ptr<FileVideoFrame>> video_frames;
    vector<unique_ptr<FileAudioFrame>> audio_frames;
    vector<unique_ptr<FileIMUFrame>> imu_frames;
    vector<unique_ptr<FileTRSFrame>> trs_frames;
    return std::make_unique<File>(*file_offsets_,
                                  *file_info_,
                                  *file_tracks_,
                                  *file_attachments_,
                                  std::move(video_frames),
                                  std::move(audio_frames),
                                  std::move(imu_frames),
                                  std::move(trs_frames));
}

unique_ptr<File> FileParser::parseAllFrames()
{
    return parseAllClusters();
}
} // namespace rgbd
