/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include <filesystem>
#include "constants.hpp"

namespace rgbd
{
// This VideoFolder class is to access the video recordings saved in /videos directory
// of the librgbd repository for testing purposes.
class VideoFolder
{
public:
    VideoFolder(const string& folder_path);
    static optional<VideoFolder> createFromDefaultPath();
    optional<std::filesystem::path> runSelectFileCLI();
    const vector<std::filesystem::path> file_paths() const noexcept
    {
        return file_paths_;
    }

private:
    vector<std::filesystem::path> file_paths_;
};
} // namespace rgbd
