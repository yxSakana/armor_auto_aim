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
    using UpdateFunction = std::function<Eigen::MatrixXd(const Eigen::VectorXd&)>;
    using UpdateFunctionVoid = std::function<Eigen::MatrixXd()>;
public:
    ExtendedKalmanFilter(const Eigen::MatrixXd& P0,
                         const NonlinearFunction& _f, const NonlinearFunction& _h,
                         const UpdateFunction& jacobian_f, const UpdateFunction& jacobian_h,
                         const UpdateFunctionVoid& update_q, const UpdateFunction& update_r);

    Eigen::MatrixXd update();

    Eigen::MatrixXd predict(const Eigen::VectorXd& z);

    const Eigen::VectorXd& getX() const { return X_prior; }
    [[nodiscard]] const Eigen::MatrixXd& getP() const { return P_posterior; };
    [[nodiscard]] const Eigen::MatrixXd& getH() const { return H; }
private:
    int n;  // System dimensions(系统尺寸)

    NonlinearFunction f;
    UpdateFunction jacobian_f;
    UpdateFunction jacobian_h;
    Eigen::VectorXd X_prior;
    Eigen::VectorXd X_posterior;

    Eigen::MatrixXd F;
    Eigen::MatrixXd P_prior;
    Eigen::MatrixXd P_posterior;
    Eigen::MatrixXd Q;  // Q is W * Q * W^T
    UpdateFunctionVoid update_Q;

    Eigen::MatrixXd K;
    Eigen::MatrixXd I;

    Eigen::MatrixXd H;
    Eigen::MatrixXd R;  // R is V * R * V^T
    UpdateFunction update_R;
    NonlinearFunction h;
};
}

#endif //TEST_EXTENDED_KALMAN_FILTER_EXTENDED_KALMAN_FILTER_H
