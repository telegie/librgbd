/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "video_folder.hpp"
#include <iostream>

namespace rgbd
{
VideoFolder::VideoFolder(const string& folder_path)
    : file_paths_()
{
    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
        if (entry.is_directory())
            continue;

        std::filesystem::path file_path{entry.path()};
        if (file_path.filename().string() == ".gitignore")
            continue;
        if (!file_path.has_extension())
            continue;
        if (file_path.extension().string() != ".mkv")
            continue;
        file_paths_.push_back(file_path);
    }
}

optional<VideoFolder> VideoFolder::createFromDefaultPath()
{
    string video_directory_path{CMAKE_RGBD_VIDEOS_DIR};

    if (std::filesystem::exists(video_directory_path)) {
        return VideoFolder{video_directory_path};
    }

    return nullopt;
}

optional<std::filesystem::path> VideoFolder::runSelectFileCLI()
{
    if (file_paths_.size() == 0) {
        spdlog::error("No file in the /videos folder to select.");
        return nullopt;
    }

    std::cout << "Video File Options:" << std::endl;
    for (int i{0}; i < file_paths_.size(); ++i)
        std::cout << "- (" << i << ") " << file_paths_[i].filename().string() << std::endl;

    int selected_index{0};
    while (true) {
        std::cout << "Select: ";
        string line;
        std::getline(std::cin, line);
        try {
            selected_index = stoi(line);
            if (selected_index >= 0 && selected_index < file_paths_.size())
                break;
        } catch (std::invalid_argument) {
            // Ignore non-numeric inputs.
        }
        std::cout << "Invalid Input...\n";
    }

    return file_paths()[selected_index];
}
} // namespace tg
