/**
 * @projectName armor_auto_aim
 * @file pnp_solver.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-22 16:39
 */

#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>

#include <solver/pnp_solver.h>

namespace armor_auto_aim {
PnPSolver::PnPSolver(const std::array<double, 9>& intrinsic_matrix,
                     const std::vector<double>& distortion_vector)
                     : m_intrinsic_matrix(cv::Mat(3, 3, CV_64F, const_cast<double*>(intrinsic_matrix.data())).clone()),
                       m_distortion_vector(cv::Mat(1, 5, CV_64F, const_cast<double*>(distortion_vector.data())).clone())
{
    // Unit: m
    constexpr double small_half_x = SMALL_ARMOR_WIDTH / 2.0 / 1000.0;
    constexpr double small_half_y = SMALL_ARMOR_HEIGHT / 2.0 / 1000.0;
    constexpr double large_half_x = LARGE_ARMOR_WIDTH / 2.0 / 1000.0;
    constexpr double large_half_y = LARGE_ARMOR_HEIGHT / 2.0 / 1000.0;

    // 3d Points(lt lb rb rt)
    m_small_armor_point3d.push_back(cv::Point3f(small_half_x, small_half_y, 0));
    m_small_armor_point3d.push_back(cv::Point3f(small_half_x, -small_half_y, 0));
    m_small_armor_point3d.push_back(cv::Point3f(-small_half_x, -small_half_y, 0));
    m_small_armor_point3d.push_back(cv::Point3f(-small_half_x, small_half_y, 0));

    m_large_armor_point3d.push_back(cv::Point3f(large_half_x, large_half_y, 0));
    m_large_armor_point3d.push_back(cv::Point3f(large_half_x, -large_half_y, 0));
    m_large_armor_point3d.push_back(cv::Point3f(-large_half_x, -large_half_y, 0));
    m_large_armor_point3d.push_back(cv::Point3f(-large_half_x, large_half_y, 0));
}

bool PnPSolver::pnpSolver(const Armor& armor, cv::Mat& rvec, cv::Mat& tvec) {
    auto point3d = armor.type == ArmorType::SMALL? m_small_armor_point3d: m_large_armor_point3d;
    return cv::solvePnP(point3d, armor.armor_apex, m_intrinsic_matrix, m_distortion_vector,
                        rvec, tvec, false, cv::SOLVEPNP_IPPE);  // cv::SOLVEPNP_ITERATIVE ?
}

bool PnPSolver::obtain3dPose(Armor& armor) {
    cv::Mat rvec, tvec;
    if (pnpSolver(armor, rvec, tvec)) {
        auto correctEulerAngles = [](Eigen::Vector3d& euler_angles) {
//            euler_angles(2) = euler_angles(2) > 0? euler_angles(2) - M_PI: euler_angles(2) + M_PI;
            if (euler_angles(2) > M_PI_2) {
                euler_angles(2) = M_PI - euler_angles(2);
            } else if (euler_angles(2) < -M_PI_2) {
                euler_angles(2) = -M_PI - euler_angles(2);
            }
            if (euler_angles(1) > M_PI_2) {
                euler_angles(1) = M_PI - euler_angles(1);
            } else if (euler_angles(1) < -M_PI_2) {
                euler_angles(1) = -M_PI - euler_angles(1);
            }
        };  // 范围
        cv::Mat rmat_cv;
        cv::Rodrigues(rvec, rmat_cv);
        Eigen::Matrix3d rmat_eigen;
        cv::cv2eigen(rmat_cv, rmat_eigen);
        Eigen::Vector3d euler_angles =rmat_eigen.eulerAngles(2, 1, 0);

        armor.pose.pitch = static_cast<float>(euler_angles(1));
        armor.pose.yaw = static_cast<float>(euler_angles(2));
        armor.pose.roll = static_cast<float>(euler_angles(0));
        armor.pose.x = static_cast<float>(tvec.at<double>(0, 0));
        armor.pose.y = static_cast<float>(tvec.at<double>(1, 0));
        armor.pose.z = static_cast<float>(tvec.at<double>(2, 0));

        return true;
    } else {
        return false;
    }
}
} // armor_auto_aim