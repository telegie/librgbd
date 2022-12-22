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

glm::mat3x3 random_mat3x3()
{
    glm::mat3x3 mat;
    mat[0][0] = random_float();
    mat[1][0] = random_float();
    mat[2][0] = random_float();
    mat[0][1] = random_float();
    mat[1][1] = random_float();
    mat[2][1] = random_float();
    mat[0][2] = random_float();
    mat[1][2] = random_float();
    mat[2][2] = random_float();
    return mat;
}

glm::vec3 random_vec3()
{
    return glm::vec3{random_float(), random_float(), random_float()};
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
    rgbd::KinectCameraCalibration calibration{random_calibration()};
    rgbd::KinectCameraCalibration different_calibration{random_calibration()};
    Bytes bytes{calibration.toBytes()};
    int cursor{0};
    auto deserialized_calibration{rgbd::KinectCameraCalibration::fromBytes(bytes, cursor)};
    REQUIRE(calibration == calibration);
    REQUIRE(!(calibration == different_calibration));
    REQUIRE(calibration == deserialized_calibration);
}
