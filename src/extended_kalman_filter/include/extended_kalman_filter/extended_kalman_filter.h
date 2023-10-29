/**
 * @projectName test_extended_kalman_filter
 * @file extended_kalman_filter.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-21 10:42
 */

#ifndef TEST_EXTENDED_KALMAN_FILTER_EXTENDED_KALMAN_FILTER_H
#define TEST_EXTENDED_KALMAN_FILTER_EXTENDED_KALMAN_FILTER_H

#include <functional>

#include <Eigen/Dense>

namespace armor_auto_aiming {
class ExtendedKalmanFilter {
    using NonlinearFunction = std::function<Eigen::VectorXd(const Eigen::VectorXd&)>;
    using CovarianceMatrixFunction = std::function<Eigen::MatrixXd(const Eigen::VectorXd&)>;
public:
//    ExtendedKalmanFilter(const NonlinearFunction& _f, const);
private:
    NonlinearFunction f;
    Eigen::VectorXd X_prior;
    Eigen::VectorXd X_posterior;

    Eigen::MatrixXd F;
    Eigen::MatrixXd P_prior;
    Eigen::MatrixXd P_posterior;
    Eigen::MatrixXd Q;  // Q is W * Q * W^T
    CovarianceMatrixFunction update_Q;

    Eigen::MatrixXd K;
    Eigen::MatrixXd I;
    int n;  // System dimensions(系统尺寸)

    Eigen::MatrixXd R;  // R is V * R * V^T
    CovarianceMatrixFunction update_R;
    NonlinearFunction h;
};
}

#endif //TEST_EXTENDED_KALMAN_FILTER_EXTENDED_KALMAN_FILTER_H
