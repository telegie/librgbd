#pragma once

#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <gsl/gsl>
#pragma warning(push)
#pragma warning(disable : 26812)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace tg
{
using std::byte;
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

using Bytes = vector<std::byte>;

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
}