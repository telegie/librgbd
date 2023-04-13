#pragma once

#include "record.hpp"
#include "ios_camera_calibration.hpp"
#include "kinect_camera_calibration.hpp"

namespace rgbd
{

class RecordParser
{
public:
    RecordParser(const void* ptr, size_t size);
    RecordParser(const string& file_path);

private:
    void parseExceptClusters();
    optional<const RecordInfo> parseInfo(unique_ptr<libmatroska::KaxInfo>& kax_info);
    optional<const RecordOffsets> parseOffsets(unique_ptr<libmatroska::KaxSegment>& segment);
    optional<const RecordTracks> parseTracks(unique_ptr<libmatroska::KaxTracks>& tracks);
    optional<const RecordAttachments>
    parseAttachments(unique_ptr<libmatroska::KaxAttachments>& attachments);
    RecordFrame* parseCluster(unique_ptr<libmatroska::KaxCluster>& cluster);
    void parseAllClusters(vector<RecordVideoFrame>& video_frames,
                          vector<RecordAudioFrame>& audio_frames,
                          vector<RecordIMUFrame>& imu_frames,
                          vector<RecordPoseFrame>& trs_frames,
                          vector<RecordCalibrationFrame>& calibration_frames);

public:
    unique_ptr<Record> parse(bool with_frames, bool with_directions);

private:
    unique_ptr<libebml::IOCallback> input_;
    EbmlStream stream_;
    unique_ptr<libmatroska::KaxSegment> kax_segment_;
    optional<RecordOffsets> file_offsets_;
    optional<RecordInfo> file_info_;
    optional<RecordTracks> file_tracks_;
    optional<RecordAttachments> file_attachments_;
};
} // namespace rgbd
