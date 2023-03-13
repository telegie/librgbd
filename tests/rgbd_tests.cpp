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
    for (size_t i{0}; i < depth_values.size(); ++i)
        REQUIRE(depth_values[i] == values[i]);
}

TEST_CASE("Check Euler Angles <-> Quaternion")
{
    for (int i{0}; i < 1000; ++i) {
//        glm::vec3 eulers1{random_float(), random_float(), random_float()};
        std::uniform_real_distribution distr(-10.0f, 10.0f);
        glm::vec3 eulers1{distr(eng), distr(eng), distr(eng)};
        glm::quat quat1{MathUtils::convertEulerAnglesToQuaternion(eulers1)};
        glm::vec3 eulers2{MathUtils::convertQuaternionToEulerAngles(quat1)};
        glm::quat quat2{MathUtils::convertEulerAnglesToQuaternion(eulers2)};
//        spdlog::info("eulers1: {}, eulers2: {}", glm::to_string(eulers1), glm::to_string(eulers2));
//        spdlog::info("quat1: {}, quat2: {}", glm::to_string(quat1), glm::to_string(quat2));
        REQUIRE(glm::all(glm::epsilonEqual(quat1, quat2, 0.0001f)));
    }
}
