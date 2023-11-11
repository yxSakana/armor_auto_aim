/**
 * @projectName test_extended_kalman_filter
 * @file test_utils.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-22 14:50
 */


#ifndef TEST_EXTENDED_KALMAN_FILTER_UTILS_H
#define TEST_EXTENDED_KALMAN_FILTER_UTILS_H

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>

namespace armor_auto_aim {
/**
 * @brief 检查旋转矩阵是否为奇异矩阵
 *  即: 行列式 == 1? 奇异: 非 (Tips: 浮点数计算误差)
 * @param R
 * @return
 */
inline bool isRotationMatrixValid(const Eigen::Matrix3d& R) { return std::abs(R.determinant() - 1.0) < 1e-6; }

/**
 * @brief 旋转矩形(Eigen) -> 欧拉角
 *
 * @param R
 * @return
 */
Eigen::Vector3d rotationMatrixToEulerAngles(const Eigen::Matrix3d& R);

/**
 * @brief 旋转向量(cv::Mat) -> 欧拉角()
 *
 * @param rvec
 * @return Unit: radian; roll, yaw, pitch
 */
Eigen::Vector3d rotationVectorToEulerAngles(const cv::Mat& rvec);
}

#endif //TEST_EXTENDED_KALMAN_FILTER_UTILS_H
