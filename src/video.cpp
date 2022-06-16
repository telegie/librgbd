#include "video.hpp"

namespace rgbd
{
Video::Video(const shared_ptr<CameraCalibration>& camera_calibration,
             vector<unique_ptr<VideoRGBDFrame>>&& rgbd_frames,
             vector<unique_ptr<VideoAudioFrame>>&& audio_frames)
    : camera_calibration_{camera_calibration}
    , rgbd_frames_{std::move(rgbd_frames)}
    , audio_frames_{std::move(audio_frames)}
{
}
}
