/**
 * @projectName armor_auto_aim
 * @file armor_detector.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-27 20:00
 */

#ifndef ARMOR_AUTO_AIMING_DETECTOR_H
#define ARMOR_AUTO_AIMING_DETECTOR_H

#include <string>

#include <opencv2/core.hpp>

#include <armor_detector/interface.h>
#include <armor_detector/inference.h>
#include <solver/pnp_solver.h>

namespace armor_auto_aim {
class Detector {
public:
    Detector();

    bool detect(const cv::Mat& frame, std::vector<Armor>* armors);
private:
    const std::string m_model_path = "../model/opt-0527-001.xml";
    const std::array<double, 9> m_intrinsic_matrix {
            1664.5, -7.7583, 743.8596,
            0.0, 1669.6, 573.3738,
            0.0, 573.37, 1.0
    };
    const std::vector<double> m_distortion_vector { -0.44044, 0.2949, 0, -0.0042, 0 };

    std::unique_ptr<Inference> m_inference;
    std::unique_ptr<PnPSolver> m_pnp_solver;
};

} // armor_auto_aim

#endif //ARMOR_AUTO_AIMING_DETECTOR_H
