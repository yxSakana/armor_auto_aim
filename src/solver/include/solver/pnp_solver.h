/**
 * @projectName armor_auto_aim
 * @file pnp_solver.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-22 16:39
 */

#ifndef ARMOR_AUTO_AIMING_PNP_SOLVER_H
#define ARMOR_AUTO_AIMING_PNP_SOLVER_H

#include <opencv2/opencv.hpp>

#include <solver/coord_utils.h>
#include <armor_detector/interface.h>
#include <solver/solver_interface.h>

namespace armor_auto_aim {
class PnPSolver {
public:
    PnPSolver(const std::array<double, 9>& intrinsic_matrix,
              const std::vector<double>& distortion_vector);

    bool pnpSolver(const Armor& armor, cv::Mat& rvec, cv::Mat& tvec);

    bool obtain3dPose(Armor& armor);
private:
    // Unit: mm
    static constexpr float SMALL_ARMOR_WIDTH = 135;
    static constexpr float SMALL_ARMOR_HEIGHT = 55;
    static constexpr float LARGE_ARMOR_WIDTH = 225;
    static constexpr float LARGE_ARMOR_HEIGHT = 55;

    cv::Mat m_intrinsic_matrix;
    cv::Mat m_distortion_vector;
    std::vector<cv::Point3f> m_small_armor_point3d;
    std::vector<cv::Point3f> m_large_armor_point3d;
};

} // armor_auto_aim

#endif //ARMOR_AUTO_AIMING_PNP_SOLVER_H
