/**
 * @projectName test_extended_kalman_filter
 * @file armor_tracker.cpp
 * @brief 
 * @author yx
 * @date 2023-10-21 10:41
 */

#include <armor_tracker/extended_kalman_filter.h>

#include <glog/logging.h>

namespace armor_auto_aim {
ExtendedKalmanFilter::ExtendedKalmanFilter(
        const Eigen::MatrixXd& P0,
        const NonlinearFunction& _f, const NonlinearFunction& _h,
        const UpdateFunction& jacobian_f, const UpdateFunction& jacobian_h,
        const UpdateFunctionVoid& update_q, const UpdateFunction& update_r)
        : n(P0.rows()),
          P_posterior(P0), X_prior(n), X_posterior(n), I(Eigen::MatrixXd::Identity(n ,n)),
          f(_f), h(_h),
          jacobian_f(jacobian_f), jacobian_h(jacobian_h),
          update_Q(update_q), update_R(update_r) {
}

Eigen::MatrixXd ExtendedKalmanFilter::update() {
    F = jacobian_f(X_posterior);
    Q = update_Q();

    X_prior = f(X_posterior);
    P_prior = F * P_posterior * F.transpose() + Q;

    X_posterior = X_prior;
    P_posterior = P_prior;

    return X_prior;
}

Eigen::MatrixXd ExtendedKalmanFilter::predict(const Eigen::VectorXd& z) {
   H = jacobian_h(X_posterior);
   R = update_R(z);

   K = P_prior * H.transpose() * (H * P_prior * H.transpose() + R).inverse();
   X_posterior = X_prior + K * (z - h(X_prior));
   P_posterior = (I - K * H) * P_prior;

   return X_posterior;
}

void ExtendedKalmanFilter::showInfo() {
    LOG(INFO) << "X_prior: " << X_prior;
    LOG(INFO) << "X_posterior: " << X_posterior;
    LOG(INFO) << "P_prior: " << P_prior;
    LOG(INFO) << "P_posterior: " << P_posterior;
    LOG(INFO) << "F: " << F;
    LOG(INFO) << "H: " << H;
    LOG(INFO) << "Q: " << Q;
    LOG(INFO) << "R: " << R;
}
}
