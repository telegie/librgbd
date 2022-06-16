#include "file.hpp"

namespace rgbd
{
File::File(const shared_ptr<CameraCalibration>& camera_calibration,
           vector<unique_ptr<FileVideoFrame>>&& video_frames,
           vector<unique_ptr<FileAudioFrame>>&& audio_frames)
    : camera_calibration_{camera_calibration}
    , video_frames_{std::move(video_frames)}
    , audio_frames_{std::move(audio_frames)}
{
}
}
