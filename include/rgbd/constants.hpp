#pragma once

#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <gsl/gsl>
#pragma warning(push)
#pragma warning(disable : 26819 26495 28020)
#include <nlohmann/json.hpp>
#pragma warning(pop)
#pragma warning(push)
#pragma warning(disable : 26812)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace rgbd
{
// CMAKE_TS_VERSION_XXX are set via CMake.
constexpr int MAJOR_VERSION{CMAKE_RGBD_VERSION_MAJOR};
constexpr int MINOR_VERSION{CMAKE_RGBD_VERSION_MINOR};
constexpr int PATCH_VERSION{CMAKE_RGBD_VERSION_PATCH};

using std::list;
using std::map;
using std::nullopt;
using std::optional;
using std::pair;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;
using std::weak_ptr;

using gsl::span;
using nlohmann::json;

using Bytes = vector<uint8_t>;

enum class CameraDeviceType : int32_t
{
    AzureKinect = 0,
    IOS = 1,
    Undistorted = 2
};

string stringify_camera_device_type(const CameraDeviceType& camera_device_type);

enum class ColorCodecType : int32_t
{
    VP8 = 0
};

enum class DepthCodecType : int32_t
{
    RVL = 0,
    TDC1 = 1
};

constexpr int VIDEO_FRAME_RATE{30};
constexpr float DEFAULT_DEPTH_UNIT{0.001f};

/////////////////////////////////////////////////////////////
////////////////////// AUDIO CONSTANTS //////////////////////
/////////////////////////////////////////////////////////////

// The number of samples per seconds the Kinect's microphone produces.
// And also the only one that Opus supports.
constexpr int AUDIO_SAMPLE_RATE{48000};

// We will use mono audio when sending them from a sender to a receiver.
constexpr int AUDIO_INPUT_CHANNEL_COUNT{1};

// 960 is for 0.02 seconds with the given sample rate (i.e., 48000)
// Also, this is the frame_size accepted by Opus in 48 kHz.
constexpr int AUDIO_INPUT_SAMPLES_PER_FRAME{960};

// Play audio in stereo.
constexpr int AUDIO_OUTPUT_CHANNEL_COUNT{2};

// 1 / 0.02 seconds.
constexpr int AUDIO_OUTPUT_INTERVAL_SECONDS_RECIPROCAL{50};

// 960 is for 0.02 seconds with the given sample rate (i.e., 48000)
constexpr int AUDIO_OUTPUT_SAMPLES_PER_FRAME{AUDIO_SAMPLE_RATE * AUDIO_OUTPUT_CHANNEL_COUNT /
                                             AUDIO_OUTPUT_INTERVAL_SECONDS_RECIPROCAL};

// 1 / 0.2 seconds.
constexpr int DEFAULT_AUDIO_RING_BUFFER_SECONDS_RECIPROCAL{5};

// Divided with sizeof(float) as the ring buffer holds floats, not bytes.
constexpr int DEFAULT_AUDIO_RING_BUFFER_SIZE{AUDIO_SAMPLE_RATE * AUDIO_INPUT_CHANNEL_COUNT /
                                             DEFAULT_AUDIO_RING_BUFFER_SECONDS_RECIPROCAL};


/////////////////////////////////////////////////////////////
/////////////////////// FILE CONSTANTS //////////////////////
/////////////////////////////////////////////////////////////
constexpr int64_t ONE_SECOND_NS{1000 * 1000 * 1000}; // in ns
constexpr int64_t ONE_MICROSECOND_NS{1000};          // in ns
constexpr int MATROSKA_TIMESCALE_NS{ONE_MICROSECOND_NS};
} // namespace rgbd