#pragma once

#include "file_writer.hpp"

namespace rgbd
{
// FileWriterHelper is to make using FileWriter easier.
class RecordBytesBuilder
{
public:
    RecordBytesBuilder();
    void setSampleRate(int sample_rate);
    void setDepthCodecType(DepthCodecType depth_codec_type);
    void setDepthUnit(float depth_unit);
    void setCalibration(const CameraCalibration& calibration);
    void setCoverPNGBytes(const optional<Bytes>& cover_png_bytes);
    void addVideoFrame(const RecordVideoFrame& video_frame);
    void addAudioFrame(const RecordAudioFrame& audio_frame);
    void addIMUFrame(const RecordIMUFrame& imu_frame);
    void addTRSFrame(const RecordTRSFrame& trs_frame);
    Bytes build();
    void buildToPath(const std::string& path);

private:
    void _build(IOCallback& io_callback);

private:
    int sample_rate_;
    DepthCodecType depth_codec_type_;
    float depth_unit_;
    unique_ptr<CameraCalibration> calibration_;
    optional<Bytes> cover_png_bytes_;
    vector<RecordVideoFrame> video_frames_;
    vector<RecordAudioFrame> audio_frames_;
    vector<RecordIMUFrame> imu_frames_;
    vector<RecordTRSFrame> trs_frames_;
};
}