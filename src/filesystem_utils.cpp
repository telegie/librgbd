/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "filesystem_utils.hpp"

namespace rgbd
{
optional<vector<string>> get_filenames_from_folder_path(const string folder_path)
{
    if (!std::filesystem::exists(folder_path))
        return nullopt;

    vector<string> filenames;
    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
        const string filename{entry.path().filename().string()};
        if (filename == ".gitignore")
            continue;
        if (entry.is_directory())
            continue;
        filenames.push_back(filename);
    }

    return filenames;
}

optional<DataFolder> FileSystemUtils::findDataFolder(const gsl::span<const string> folder_paths)
{
    for (const auto& folder_path : folder_paths) {
        const auto filenames{get_filenames_from_folder_path(folder_path)};
        if (filenames)
            return DataFolder{folder_path, *filenames};
    }

    return nullopt;
}
} // namespace tg
