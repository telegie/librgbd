/**
 * Copyright (c) 2020-2021, Hanseul Jun.
 */

#include "kinect_calibration_utils.hpp"

namespace tg
{
glm::vec2 transformation_project_internal(const KinectCameraCalibration& calibration,
                                          const glm::vec2& xy,
                                          bool& valid,
                                          float J_xy[2 * 2])
{
    glm::vec2 uv{0.0f, 0.0f};
    float cx = calibration.cx();
    float cy = calibration.cy();
    float fx = calibration.fx();
    float fy = calibration.fy();
    float k1 = calibration.k1();
    float k2 = calibration.k2();
    float k3 = calibration.k3();
    float k4 = calibration.k4();
    float k5 = calibration.k5();
    float k6 = calibration.k6();
    float codx = calibration.codx(); // center of distortion is set to 0 for Brown Conrady model
    float cody = calibration.cody();
    float p1 = calibration.p1();
    float p2 = calibration.p2();
    float max_radius_for_projection = calibration.max_radius_for_projection();

    if (fx <= 0.f || fy <= 0.f)
        throw std::runtime_error("Expect both fx and fy are larger than 0");

    valid = true;

    float xp = xy.x - codx;
    float yp = xy.y - cody;

    float xp2 = xp * xp;
    float yp2 = yp * yp;
    float xyp = xp * yp;
    float rs = xp2 + yp2;
    if (rs > max_radius_for_projection * max_radius_for_projection) {
        valid = false;
        return uv;
    }
    float rss = rs * rs;
    float rsc = rss * rs;
    float a = 1.f + k1 * rs + k2 * rss + k3 * rsc;
    float b = 1.f + k4 * rs + k5 * rss + k6 * rsc;
    float bi;
    if (b != 0.f) {
        bi = 1.f / b;
    } else {
        bi = 1.f;
    }
    float d = a * bi;

    float xp_d = xp * d;
    float yp_d = yp * d;

    float rs_2xp2 = rs + 2.f * xp2;
    float rs_2yp2 = rs + 2.f * yp2;

    xp_d += rs_2xp2 * p2 + 2.f * xyp * p1;
    yp_d += rs_2yp2 * p1 + 2.f * xyp * p2;

    float xp_d_cx = xp_d + codx;
    float yp_d_cy = yp_d + cody;

    uv.x = xp_d_cx * fx + cx;
    uv.y = yp_d_cy * fy + cy;

    if (J_xy == 0)
        return uv;

    // compute Jacobian matrix
    float dudrs = k1 + 2.f * k2 * rs + 3.f * k3 * rss;
    // compute d(b)/d(r^2)
    float dvdrs = k4 + 2.f * k5 * rs + 3.f * k6 * rss;
    float bis = bi * bi;
    float dddrs = (dudrs * b - a * dvdrs) * bis;

    float dddrs_2 = dddrs * 2.f;
    float xp_dddrs_2 = xp * dddrs_2;
    float yp_xp_dddrs_2 = yp * xp_dddrs_2;
    // compute d(u)/d(xp)
    J_xy[0] = fx * (d + xp * xp_dddrs_2 + 6.f * xp * p2 + 2.f * yp * p1);
    J_xy[1] = fx * (yp_xp_dddrs_2 + 2.f * yp * p2 + 2.f * xp * p1);
    J_xy[2] = fy * (yp_xp_dddrs_2 + 2.f * xp * p1 + 2.f * yp * p2);
    J_xy[3] = fy * (d + yp * yp * dddrs_2 + 6.f * yp * p1 + 2.f * xp * p2);

    return uv;
}

void invert_2x2(const float J[2 * 2], float Jinv[2 * 2])
{
    float detJ = J[0] * J[3] - J[1] * J[2];
    float inv_detJ = 1.f / detJ;

    Jinv[0] = inv_detJ * J[3];
    Jinv[3] = inv_detJ * J[0];
    Jinv[1] = -inv_detJ * J[1];
    Jinv[2] = -inv_detJ * J[2];
}

void transformation_iterative_unproject(const KinectCameraCalibration& calibration,
                                        const glm::vec2& uv,
                                        glm::vec2& xy,
                                        bool& valid,
                                        unsigned int max_passes)
{
    valid = true;
    float Jinv[2 * 2];
    float best_xy[2] = {0.f, 0.f};
    float best_err = FLT_MAX;

    for (unsigned int pass = 0; pass < max_passes; pass++) {
        float J[2 * 2];
        glm::vec2 p{transformation_project_internal(calibration, xy, valid, J)};
        if (!valid)
            return;

        float err_x = uv.x - p.x;
        float err_y = uv.y - p.y;
        float err = err_x * err_x + err_y * err_y;
        if (err >= best_err) {
            xy.x = best_xy[0];
            xy.y = best_xy[1];
            break;
        }

        best_err = err;
        best_xy[0] = xy[0];
        best_xy[1] = xy[1];
        invert_2x2(J, Jinv);
        if (pass + 1 == max_passes || best_err < 1e-22f) {
            break;
        }

        float dx = Jinv[0] * err_x + Jinv[1] * err_y;
        float dy = Jinv[2] * err_x + Jinv[3] * err_y;

        xy[0] += dx;
        xy[1] += dy;
    }

    if (best_err > 1e-6f)
        valid = false;
}

glm::vec2 transformation_unproject_internal(const KinectCameraCalibration& calibration,
                                            const glm::vec2& uv,
                                            bool& valid)
{
    float cx = calibration.cx();
    float cy = calibration.cy();
    float fx = calibration.fx();
    float fy = calibration.fy();
    float k1 = calibration.k1();
    float k2 = calibration.k2();
    float k3 = calibration.k3();
    float k4 = calibration.k4();
    float k5 = calibration.k5();
    float k6 = calibration.k6();
    float codx = calibration.codx(); // center of distortion is set to 0 for Brown Conrady model
    float cody = calibration.cody();
    float p1 = calibration.p1();
    float p2 = calibration.p2();

    if (fx <= 0.f && fy <= 0.f)
        throw std::runtime_error("Expect both fx and fy are larger than 0");

    // correction for radial distortion
    float xp_d = (uv.x - cx) / fx - codx;
    float yp_d = (uv.y - cy) / fy - cody;

    float rs = xp_d * xp_d + yp_d * yp_d;
    float rss = rs * rs;
    float rsc = rss * rs;
    float a = 1.f + k1 * rs + k2 * rss + k3 * rsc;
    float b = 1.f + k4 * rs + k5 * rss + k6 * rsc;
    float ai;
    if (a != 0.f) {
        ai = 1.f / a;
    } else {
        ai = 1.f;
    }
    float di = ai * b;

    glm::vec2 xy;
    xy.x = xp_d * di;
    xy.y = yp_d * di;

    // approximate correction for tangential params
    float two_xy = 2.f * xy.x * xy.y;
    float xx = xy.x * xy.x;
    float yy = xy.y * xy.y;

    xy.x -= (yy + 3.f * xx) * p2 + two_xy * p1;
    xy.y -= (xx + 3.f * yy) * p1 + two_xy * p2;

    // add on center of distortion
    xy.x += codx;
    xy.y += cody;

    transformation_iterative_unproject(calibration, uv, xy, valid, 20);
    return xy;
}

glm::vec3 kinect_transformation_unproject(const KinectCameraCalibration& calibration,
                                          const glm::vec2& point2d,
                                          bool& valid)
{
    glm::vec2 point3d_xy{transformation_unproject_internal(calibration, point2d, valid)};
    return glm::vec3{point3d_xy.x, point3d_xy.y, 1.0f};
}

glm::vec3 compute_kinect_direction(const KinectCameraCalibration& calibration, const glm::vec2& uv)
{
    const int width{calibration.resolution_width()};
    const int height{calibration.resolution_height()};

    glm::vec2 point2d{uv.x * (width - 1), uv.y * (height - 1)};
    bool valid;
    glm::vec3 point3d{
        kinect_transformation_unproject(calibration, point2d, valid)};

    // Flipping y and z to convert from the Kinect's coordiante system to Krypton's.
    // Kinect's: https://docs.microsoft.com/en-us/azure/kinect-dk/coordinate-systems
    // Krypton has x: right, y: up, and is a right-hand coordinate system (i.e., z: back).
    return glm::vec3{point3d.x, -point3d.y, -point3d.z};
}
} // namespace tg
