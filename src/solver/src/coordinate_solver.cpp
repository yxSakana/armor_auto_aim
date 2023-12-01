/**
 * @project_name auto_aim
 * @file coordinate_solver.cpp
 * @brief
 * @author yx
 * @data 2023-11-26 14:14:15
 */

#include <solver/coordinate_solver.h>

namespace armor_auto_aim::coordinate_solver {
cv::Point2d reproject(const Eigen::Matrix3d& camera_intrinsic, const Eigen::Vector3d& xyz) {
    Eigen::Vector3d result = camera_intrinsic * xyz * (1.0f / xyz[2]);
    return {result[0], result[1]};
}

cv::Point2d reproject(const std::array<double, 9>& camera_intrinsic, const Eigen::Vector3d& xyz) {
    Eigen::Matrix3d camera_intrinsic_eigen;
    for (int i = 0; i < 9; ++i)
        camera_intrinsic_eigen(i / 3, i % 3) = camera_intrinsic[i];
    return reproject(camera_intrinsic_eigen, xyz);
}
}
