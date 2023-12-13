/**
 * @projectName test_extended_kalman_filter
 * @file coord_utils.cpp
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

/**
 * @brief 最小角度差
 * @param from
 * @param to
 * @return Range: [-pi, pi]
 */
inline static double shortestAngularDistance(double from, double to) {
    double delta = to - from;
    delta = fmod(delta + M_PI, 2 * M_PI) - M_PI;
    return delta;
}

inline double angleToRadian(double angel) { return (angel * M_PI) / 180; }

inline double radianToAngel(double radian) { return (radian * 180) / M_PI; }

std::string to_string(const Eigen::MatrixXd& matrix);

inline void getYawPitchDis(const Eigen::Vector3d& translation, float& yaw, float& pitch, float& distance) {
    yaw = static_cast<float>(atan2(translation(0), translation(2)) * 180.0f / M_PI),
    pitch = static_cast<float>(atan2(translation(1), translation(2)) * 180.0f / M_PI),
    distance = static_cast<float>(translation(2));
}
}

#endif //TEST_EXTENDED_KALMAN_FILTER_UTILS_H
