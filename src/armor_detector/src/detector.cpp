/**
 * @projectName armor_auto_aim
 * @file armor_detector.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-27 20:00
 */

#include <armor_detector/detector.h>

namespace armor_auto_aim {
Detector::Detector()
    : m_inference(std::make_unique<Inference>(m_model_path)),
      m_pnp_solver(std::make_unique<PnPSolver>(m_intrinsic_matrix, m_distortion_vector)) {}

bool Detector::detect(const cv::Mat& frame, std::vector<Armor>* armors) {
    armors->clear();
    bool is_ok = false;

    std::vector<InferenceResult> inference_result;
    bool status = m_inference->inference(frame, &inference_result);
    if (status) {
        for (int i = 0; i < inference_result.size(); ++i) {
            armors->emplace_back(armor_auto_aim::InferenceResult(inference_result[i]));
            is_ok = m_pnp_solver->obtain3dPose((*armors)[i], (*armors)[i].pose);
            if (!is_ok)
                armors->pop_back();
        }
        return !armors->empty();
    } else {
        return false;
    }
}
} // armor_auto_aim