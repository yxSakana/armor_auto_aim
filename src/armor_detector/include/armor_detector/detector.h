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
            2665.005527408399, 0,                  696.8233, // fx 0  cx
            0,                 2673.364537791387,  500.5147099572225, // 0  fy cy
            0,                 0,                  1
    };
    const std::vector<double> m_distortion_vector {-0.303608974614145, 4.163247825941419, -0.008432853056627, -0.003830248744148, 0};

    std::unique_ptr<Inference> m_inference;
    std::unique_ptr<PnPSolver> m_pnp_solver;
};

} // armor_auto_aim

#endif //ARMOR_AUTO_AIMING_DETECTOR_H
