/**
 * @project_name auto_aim
 * @file coordinate_solver.h
 * @brief
 * @author yx
 * @data 2023-11-26 14:14:05
 */

#ifndef AUTO_AIM_COORDINATE_SOLVER_H
#define AUTO_AIM_COORDINATE_SOLVER_H

#include <Eigen/Dense>
#include <opencv2/core.hpp>

namespace armor_auto_aim::coordinate_solver {
cv::Point2d reproject(const Eigen::Matrix3d& camera_intrinsic, const Eigen::Vector3d& xyz);

cv::Point2d reproject(const std::array<double, 9>& camera_intrinsic, const Eigen::Vector3d& xyz);
}

#endif //AUTO_AIM_COORDINATE_SOLVER_H
