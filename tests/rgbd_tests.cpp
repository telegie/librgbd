#pragma warning(push)
#include <catch2/catch_all.hpp>
#pragma warning(disable : 4201)
#include <glm/gtx/string_cast.hpp>
#pragma warning(pop)
#include <rgbd/rgbd.hpp>

using namespace rgbd;

std::random_device rd;
std::default_random_engine eng(rd());

float random_float()
{
    std::uniform_real_distribution distr(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
    return distr(eng);
}

int random_int()
{
    std::uniform_int_distribution distr(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
//    std::uniform_int_distribution distr(0, static_cast<int>(std::numeric_limits<short>::max()) * 10);
    return distr(eng);
}

KinectCameraCalibration random_calibration()
{
    return KinectCameraCalibration{
        random_int(),   random_int(),   random_int(),   random_int(),
        random_int(),   random_int(),   random_float(), random_float(),
        random_float(), random_float(), random_float(), random_float(),
        random_float(), random_float(), random_float(), random_float(),
        random_float(), random_float(), random_float(), random_float(),
        random_float()};
}

TEST_CASE("KinectCameraCalibration Serialization")
{
    KinectCameraCalibration calibration{random_calibration()};
    KinectCameraCalibration different_calibration{random_calibration()};
    Bytes bytes{calibration.toBytes()};
    int cursor{0};
    auto deserialized_calibration{KinectCameraCalibration::fromBytes(bytes, cursor)};
    REQUIRE(calibration == calibration);
    REQUIRE(!(calibration == different_calibration));
    REQUIRE(calibration == deserialized_calibration);
}

TEST_CASE("RVL Encoding")
{
    vector<int32_t> depth_values;
    for (size_t i{0}; i < 1000; ++i)
        depth_values.push_back(random_int());
    auto bytes{rvl::compress(gsl::span<const int32_t>{depth_values})};
    auto values{rvl::decompress<int32_t>(bytes, depth_values.size())};
    REQUIRE(depth_values.size() == values.size());
    for (size_t i{0}; i < depth_values.size(); ++i) {
        spdlog::info("i: {}, {}", i, depth_values[i]);
        REQUIRE(depth_values[i] == values[i]);
    }
}
