#include <rgbd/video_parser.hpp>
#include <cxxopts.hpp>

int main(int argc, char** argv)
{
    cxxopts::Options options{"rgbd-cli", "CLI for librgbd."};
    options.add_option("", cxxopts::Option{"h,help", "Print Usage"});
    options.add_option("", cxxopts::Option{"v,version", "Print Version"});

    auto result{options.parse(argc, argv)};
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    } else if (result.count("version")) {
        std::cout << fmt::format("{}.{}.{}", 1, 5, 12) << std::endl;
        return 0;
    }

    std::cout << "no option found from rgbd-cli" << std::endl;
    return 0;
}
