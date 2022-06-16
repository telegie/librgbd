#include <rgbd/file_parser.hpp>
#include <cxxopts.hpp>

int main(int argc, char** argv)
{
    cxxopts::Options options{"rgbd-cli", "CLI for librgbd."};
    options.add_option("", cxxopts::Option{"h,help", "Print Usage"});
    options.add_option("", cxxopts::Option{"v,version", "Print Version"});
    options.add_option("", cxxopts::Option{"f,file", "Print File Details", cxxopts::value<std::string>()});

    auto result{options.parse(argc, argv)};
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    } else if (result.count("version")) {
        std::cout << fmt::format("{}.{}.{}", 1, 5, 12) << std::endl;
        return 0;
    } else if (result.count("file")) {
        auto file_path{result["file"].as<std::string>()};
        rgbd::FileParser parser{file_path};
        auto& file_info{parser.info()};
        std::cout << "depth track codec: " << file_info.depth_track_info().codec << std::endl;

        if (file_info.depth_confidence_track_info()) {
            std::cout << "has depth confidence track: "
                      << file_info.depth_confidence_track_info()->codec << std::endl;
        } else {
            std::cout << "doesn't have depth confidence track" << std::endl;
        }
        return 0;
    }

    std::cout << "no option found from rgbd-cli" << std::endl;
    return 0;
}
