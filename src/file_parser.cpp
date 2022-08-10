#include "file_parser.hpp"

#include <rgbd/ios_camera_calibration.hpp>
#include <rgbd/kinect_camera_calibration.hpp>

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

FileParser::FileParser(const void* ptr, size_t size)
    : input_{new MemReadIOCallback{ptr, size}}
    , stream_{*input_}
    , info_{}
    , timecode_scale_ns_{0}
    , file_tracks_{}
    , cluster_{nullptr}
{
    init();
}

FileParser::FileParser(const string& file_path)
    : input_{new StdIOCallback{file_path.c_str(), open_mode::MODE_READ}}
    , stream_{*input_}
    , info_{}
    , timecode_scale_ns_{0}
    , file_tracks_{}
    , cluster_{nullptr}
{
    init();
}

void FileParser::init()
{
    auto head{find_next<EbmlHead>(stream_)};
    if (!head)
        throw std::runtime_error("EBML head missing");

    if (read_element<EbmlHead>(stream_, head.get()) == nullptr)
        throw std::runtime_error("Failed reading head");

    string doc_type{GetChild<EDocType>(*head).GetValue()};
    uint64_t doc_type_version{GetChild<EDocTypeVersion>(*head).GetValue()};
    uint64_t doc_type_read_version{GetChild<EDocTypeReadVersion>(*head).GetValue()};

    spdlog::info("doc_type: {}, doc_type_version: {}, doc_type_read_version: {}",
                 doc_type,
                 doc_type_version,
                 doc_type_read_version);

    auto segment{find_next<KaxSegment>(stream_, true)};
    if (!segment)
        throw std::runtime_error("No segment");

    auto element{next_child(*input_, stream_, segment.get())};
    auto file_offsets{parseOffsets(element, segment)};

    if (!file_offsets) {
        spdlog::error("Failed to parse offsets...");
        throw std::runtime_error("Failed to parse offsets...");
    }

    auto segment_info{
        read_offset<KaxInfo>(*input_, stream_, *segment, file_offsets->segment_info_offset)};

    info_.set_writing_app(GetChild<KaxWritingApp>(*segment_info).GetValue().GetUTF8());
    timecode_scale_ns_ = GetChild<KaxTimecodeScale>(*segment_info).GetValue();
    info_.set_duration_us(GetChild<KaxDuration>(*segment_info).GetValue());

    auto tracks{read_offset<KaxTracks>(*input_, stream_, *segment, file_offsets->tracks_offset)};
    file_tracks_ = parseTracks(tracks);

    if (!file_tracks_) {
        spdlog::error("Failed to parse tracks...");
        throw std::runtime_error("Failed to parse tracks...");
    }

    auto attachments{
        read_offset<KaxAttachments>(*input_, stream_, *segment, file_offsets->attachments_offset)};

    auto file_attachments{parseAttachments(attachments)};

    auto cluster{
        read_offset<KaxCluster>(*input_, stream_, *segment, file_offsets->first_cluster_offset)};

    if (!cluster)
        throw std::runtime_error("Failed to read first cluster");

    cluster_ = std::move(cluster);
}

optional<const FileOffsets> FileParser::parseOffsets(unique_ptr<EbmlElement>& element,
                                                     unique_ptr<KaxSegment>& segment)
{
    optional<int64_t> segment_info_offset{nullopt};
    optional<int64_t> tracks_offset{nullopt};
    optional<int64_t> attachments_offset{nullopt};
    optional<int64_t> first_cluster_offset{nullopt};

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
    optional<FileVideoTrack> depth_track{nullopt};
    optional<FileVideoTrack> depth_confidence_track{nullopt};
    optional<int> audio_track_number{nullopt};
    optional<int> floor_track_number{nullopt};

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
                uint64_t width{GetChild<KaxVideoPixelWidth>(track_video).GetValue()};
                uint64_t height{GetChild<KaxVideoPixelHeight>(track_video).GetValue()};

                color_track = FileVideoTrack{};
                color_track->track_number = gsl::narrow<int>(track_number);
                color_track->codec = codec_id;
                color_track->width = gsl::narrow<int>(width);
                color_track->height = gsl::narrow<int>(height);
            } else if (track_name == "DEPTH") {
                auto& track_video{GetChild<KaxTrackVideo>(*track_entry)};
                uint64_t width{GetChild<KaxVideoPixelWidth>(track_video).GetValue()};
                uint64_t height{GetChild<KaxVideoPixelHeight>(track_video).GetValue()};

                depth_track = FileVideoTrack{};
                depth_track->track_number = gsl::narrow<int>(track_number);
                depth_track->codec = codec_id;
                depth_track->width = gsl::narrow<int>(width);
                depth_track->height = gsl::narrow<int>(height);
            } else if (track_name == "DEPTH_CONFIDENCE") {
                auto& track_video{GetChild<KaxTrackVideo>(*track_entry)};
                uint64_t width{GetChild<KaxVideoPixelWidth>(track_video).GetValue()};
                uint64_t height{GetChild<KaxVideoPixelHeight>(track_video).GetValue()};

                depth_confidence_track = FileVideoTrack{};
                depth_confidence_track->track_number = gsl::narrow<int>(track_number);
                depth_confidence_track->codec = codec_id;
                depth_confidence_track->width = gsl::narrow<int>(width);
                depth_confidence_track->height = gsl::narrow<int>(height);
            } else if (track_name == "AUDIO") {
                audio_track_number = gsl::narrow<int>(track_number);
            } else if (track_name == "FLOOR") {
                floor_track_number = gsl::narrow<int>(track_number);
            } else {
                spdlog::error("Invalid track_name: {}", track_name);
            }
        } else {
            spdlog::info("is not KaxTrackEntry");
        }
    }

    if (color_track) {
        info_.set_color_track_info(*color_track);
    }
    if (depth_track) {
        info_.set_depth_track_info(*depth_track);
    }
    if (depth_confidence_track) {
        info_.set_depth_confidence_track_info(*depth_confidence_track);
    }

    if (!color_track || !depth_track || !audio_track_number || !floor_track_number)
        return nullopt;

    FileTracks file_tracks;
    file_tracks.color_track = *color_track;
    file_tracks.depth_track = *depth_track;
    file_tracks.depth_confidence_track = depth_confidence_track;
    file_tracks.audio_track_number = *audio_track_number;
    file_tracks.floor_track_number = *floor_track_number;

    return file_tracks;
}

optional<const FileAttachments>
FileParser::parseAttachments(unique_ptr<libmatroska::KaxAttachments>& attachments)
{
    shared_ptr<CameraCalibration> camera_calibration;
    Bytes cover_png_bytes;

    for (EbmlElement* e : attachments->GetElementList()) {
        if (EbmlId(*e) == KaxAttached::ClassInfos.GlobalId) {
            KaxAttached* attached_file{read_element<KaxAttached>(stream_, e)};
            if (!attached_file)
                throw std::runtime_error("Failed reading attached_file");

            auto file_name{GetChild<KaxFileName>(*attached_file).GetValue().GetUTF8()};
            spdlog::info("attached file_name: {}", file_name);
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
                } else {
                    throw std::runtime_error("Invalid calibration_type");
                }
            } else if (file_name == "cover.png") {
                spdlog::info("found cover.png");
                auto& file_data{GetChild<KaxFileData>(*attached_file)};
                cover_png_bytes = Bytes(file_data.GetSize());
                memcpy(cover_png_bytes.data(), file_data.GetBuffer(), file_data.GetSize());
            } else {
                throw std::runtime_error("Invalid attached file found");
            };
        }
    }

    if (camera_calibration)
        info_.set_camera_calibration(camera_calibration);

    if (cover_png_bytes.size() > 0)
        info_.set_cover_png_bytes(cover_png_bytes);

    if (!camera_calibration || cover_png_bytes.size() == 0)
        return nullopt;

    FileAttachments file_attachments;
    file_attachments.camera_calibration = camera_calibration;
    file_attachments.cover_png_bytes = cover_png_bytes;

    return file_attachments;
}

bool FileParser::hasNextFrame()
{
    return cluster_ != nullptr;
}

FileFrame* FileParser::readFrame()
{
    if (read_element<KaxCluster>(stream_, cluster_.get()) == nullptr)
        throw std::runtime_error{"Failed reading cluster"};
    auto cluster_timecode{FindChild<KaxClusterTimecode>(*cluster_)->GetValue()};
    cluster_->InitTimecode(cluster_timecode / timecode_scale_ns_, timecode_scale_ns_);

    int64 global_timecode{0};
    Bytes color_bytes;
    Bytes depth_bytes;
    optional<Bytes> depth_confidence_bytes{nullopt};
    optional<Plane> floor{nullopt};
    FileAudioFrame* audio_frame{nullptr};

    for (EbmlElement* e : cluster_->GetElementList()) {
        EbmlId id{*e};
        if (id == KaxClusterTimecode::ClassInfos.GlobalId) {
        } else if (id == KaxBlockGroup::ClassInfos.GlobalId) {
            auto block_group{static_cast<KaxBlockGroup*>(e)};
            if (read_element<KaxBlockGroup>(stream_, block_group) == nullptr)
                throw std::runtime_error{"Failed reading block_group"};

            for (EbmlElement* ee : block_group->GetElementList()) {
                if (EbmlId(*ee) == KaxBlock::ClassInfos.GlobalId) {
                    auto block{static_cast<KaxBlock*>(ee)};
                    block->SetParent(*cluster_);
                    auto track_number{block->TrackNum()};
                    auto block_global_timecode{gsl::narrow<int64_t>(block->GlobalTimecode())};
                    auto data_buffer{block->GetBuffer(0)};
                    if (track_number == file_tracks_->color_track.track_number) {
                        global_timecode = block_global_timecode;
                        color_bytes = copy_data_buffer_to_bytes(data_buffer);
                    } else if (track_number == file_tracks_->depth_track.track_number) {
                        depth_bytes = copy_data_buffer_to_bytes(data_buffer);
                    } else if (file_tracks_->depth_confidence_track &&
                               track_number == file_tracks_->depth_confidence_track->track_number) {
                        depth_confidence_bytes = copy_data_buffer_to_bytes(data_buffer);
                    } else if (track_number == file_tracks_->audio_track_number) {
                        global_timecode = block_global_timecode;
                        audio_frame = new FileAudioFrame{block_global_timecode,
                                                         copy_data_buffer_to_bytes(data_buffer)};
                    } else if (track_number == file_tracks_->floor_track_number) {
                        floor = Plane::fromBytes(copy_data_buffer_to_bytes(data_buffer));
                    } else {
                        throw std::runtime_error{"Invalid track number from block"};
                    }
                }
            }
        } else if (id == KaxSimpleBlock::ClassInfos.GlobalId) {
            auto simple_block{static_cast<KaxSimpleBlock*>(e)};
            simple_block->SetParent(*cluster_);
            auto track_number{simple_block->TrackNum()};
            auto block_global_timecode{gsl::narrow<int64_t>(simple_block->GlobalTimecode())};
            auto data_buffer{simple_block->GetBuffer(0)};
            if (track_number == file_tracks_->color_track.track_number) {
                global_timecode = block_global_timecode;
                color_bytes = copy_data_buffer_to_bytes(data_buffer);
            } else if (track_number == file_tracks_->depth_track.track_number) {
                depth_bytes = copy_data_buffer_to_bytes(data_buffer);
            } else if (file_tracks_->depth_confidence_track &&
                       track_number == file_tracks_->depth_confidence_track->track_number) {
                depth_confidence_bytes = copy_data_buffer_to_bytes(data_buffer);
            } else if (track_number == file_tracks_->audio_track_number) {
                audio_frame = new FileAudioFrame{block_global_timecode,
                                                 copy_data_buffer_to_bytes(data_buffer)};
            } else if (track_number == file_tracks_->floor_track_number) {
                floor = Plane::fromBytes(copy_data_buffer_to_bytes(data_buffer));
            } else {
                throw std::runtime_error{"Invalid track number from simple_block"};
            }
        } else {
            throw std::runtime_error{"Invalid element from KaxCluster"};
        }
    }

    cluster_ = find_next<KaxCluster>(stream_, true);

    // emplace only when the cluster is for video, not audio.
    if (color_bytes.size() > 0) {
        if (!floor)
            throw std::runtime_error{"Failed to find a floor"};

        return new FileVideoFrame{
            global_timecode, color_bytes, depth_bytes, depth_confidence_bytes, *floor};
    }

    if (audio_frame) {
        return audio_frame;
    }

    throw std::runtime_error{"No frame from RecordParser::readFrame"};
}

unique_ptr<File> FileParser::readAll()
{
    vector<unique_ptr<FileVideoFrame>> rgbd_frames;
    vector<unique_ptr<FileAudioFrame>> audio_frames;

    while (hasNextFrame()) {
        auto frame{readFrame()};
        switch (frame->getType()) {
        case FileFrameType::RGBD: {
            auto rgbd_frame{dynamic_cast<FileVideoFrame*>(frame)};
            rgbd_frames.emplace_back(rgbd_frame);
            break;
        }
        case FileFrameType::Audio: {
            auto audio_frame{dynamic_cast<FileAudioFrame*>(frame)};
            audio_frames.emplace_back(audio_frame);
            break;
        }
        default:
            throw std::runtime_error{"Invalid RecordFrameType found in RecordParser::readFrames"};
        }
    }

    return std::make_unique<File>(
        info_.camera_calibration(), std::move(rgbd_frames), std::move(audio_frames));
}
} // namespace rgbd
