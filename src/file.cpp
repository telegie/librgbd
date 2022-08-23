#include "file.hpp"

namespace rgbd
{
File::File(const FileOffsets& offsets,
           const FileInfo& info,
           const FileTracks& tracks,
           const FileAttachments& attachments,
           vector<unique_ptr<FileVideoFrame>>&& video_frames,
           vector<unique_ptr<FileAudioFrame>>&& audio_frames,
           vector<unique_ptr<FileIMUFrame>>&& imu_frames)
    : offsets_{offsets}
    , info_{info}
    , tracks_{tracks}
    , attachments_{attachments}
    , video_frames_{std::move(video_frames)}
    , audio_frames_{std::move(audio_frames)}
    , imu_frames_{std::move(imu_frames)}
{
}
}
