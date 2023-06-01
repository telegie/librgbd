#include <spdlog/spdlog.h>
//#include <rgbd/audio_decoder.hpp>
#include <rgbd/rgbd_capi.h>

int main()
{
    spdlog::info("hey");
    //spdlog::info("rgbd::MAJOR_VERSION: {}", rgbd::MINOR_VERSION);
    auto audio_decoder{rgbd_audio_decoder_ctor()};
    //auto audio_decoder{rgbd::AudioDecoder()};
    return 0;
}
