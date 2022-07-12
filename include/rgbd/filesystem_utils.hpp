/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#pragma once

#include "constants.hpp"
#include <filesystem>

namespace rgbd
{
struct DataFolder
{
    string folder_path;
    vector<string> filenames;

    DataFolder(const string folder_path, const vector<string>& filenames)
        : folder_path{folder_path}
        , filenames{filenames}
    {
    }
};

class FileSystemUtils
{
public:
    FileSystemUtils() = delete;
    static optional<DataFolder> findDataFolder(gsl::span<const string> folder_paths);
};
} // namespace tg
