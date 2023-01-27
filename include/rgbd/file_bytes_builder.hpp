#pragma once

#include "file_writer.hpp"

namespace rgbd
{
// FileWriterHelper is to make using FileWriter easier.
class FileBytesBuilder
{
public:
    FileBytesBuilder();
    void setFramerate(int framerate);
    void setSamplerate(int samplerate);
    void setDepthCodecType(DepthCodecType depth_codec_type);
    void setDepthUnit(float depth_unit);
    void setCalibration(const CameraCalibration& calibration);
    void setCoverPNGBytes(const Bytes& cover_png_bytes);
    void addVideoFrame(const FileVideoFrame& video_frame);
    void addAudioFrame(const FileAudioFrame& audio_frame);
    void addIMUFrame(const FileIMUFrame& imu_frame);
    void addTRSFrame(const FileTRSFrame& trs_frame);
    Bytes build();
    void buildToPath(const std::string& path);

private:
    void _build(IOCallback& io_callback);

private:
    int framerate_;
    int samplerate_;
    DepthCodecType depth_codec_type_;
    float depth_unit_;
    unique_ptr<CameraCalibration> calibration_;
    optional<Bytes> cover_png_bytes_;
    vector<FileVideoFrame> video_frames_;
    vector<FileAudioFrame> audio_frames_;
    vector<FileIMUFrame> imu_frames_;
    vector<FileTRSFrame> trs_frames_;
};
}