/**
 * @project_name auto_aim
 * @file coordinate_solver.h
 * @brief
 * @author yx
 * @date 2023-11-26 14:14:05
 */

#ifndef AUTO_AIM_COORDINATE_SOLVER_H
#define AUTO_AIM_COORDINATE_SOLVER_H

#include <Eigen/Dense>
#include <opencv2/core.hpp>

namespace armor_auto_aim::coordinate_solver {
/**
 * @brief
 * @param camera_intrinsic 相机内参
 * @param xyz 相机位姿-平移向量
 * @return
 */
cv::Point2d reproject(const Eigen::Matrix3d& camera_intrinsic, const Eigen::Vector3d& xyz);
cv::Point2d reproject(const std::array<double, 9>& camera_intrinsic, const Eigen::Vector3d& xyz);

Eigen::Vector3d cameraToImu(const Eigen::Vector3d& o1c_point,
                            const Eigen::MatrixXd& transform_c2i);

Eigen::Vector3d imuToCamera(const Eigen::Vector3d& o1i_point,
                            const Eigen::MatrixXd& transform_i2c);

/**
 * @brief 相机坐标系到(imu参考的)世界坐标系
 * @param o1c_point 装甲板在相机坐标系下的坐标
 * @param imu_rmat imu四元数解算出的旋转矩阵
 * @param transform_c2i 相机 -> imu 变换矩阵
 * @param tvec_i2w imu -> 世界坐标系(云台转轴) 平移向量
 * @return (imu参考的)世界坐标系下的坐标
 */
Eigen::Vector3d cameraToWorld(const Eigen::Vector3d& o1c_point,
                              const Eigen::Matrix3d& imu_rmat,
                              const Eigen::Matrix4d& transform_c2i,
                              const Eigen::Vector3d& tvec_i2w);

Eigen::Vector3d worldToCamera(const Eigen::Vector3d& o1w_point,
                              const Eigen::Matrix3d& imu_rmat,
                              const Eigen::Matrix4d& transform_i2c,
                              const Eigen::Vector3d& tvec_w2i);
}

#endif //AUTO_AIM_COORDINATE_SOLVER_H
