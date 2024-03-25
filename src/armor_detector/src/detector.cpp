/**
 * @projectName armor_auto_aim
 * @file armor_detector.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-27 20:00
 */

#include <yaml-cpp/yaml.h>

#include <armor_detector/detector.h>
#include <solver/coordinate_solver.h>

#include <utility>

namespace armor_auto_aim {
Detector::Detector(const std::string& model_filename,
                   const std::array<double, 9>& intrinsic_matrix,
                   const std::vector<double>& distortion_cess)
        : m_model_path(model_filename),
          m_intrinsic_matrix(intrinsic_matrix),
          m_distortion_vector(distortion_cess),
          m_inference(std::make_unique<Inference>(m_model_path)),
          m_pnp_solver(std::make_shared<PnPSolver>(m_intrinsic_matrix, m_distortion_vector)) {}

Detector::Detector(const std::string& model_filename,
                   std::shared_ptr<PnPSolver> pnp_solver_ptr)
        : m_model_path(model_filename),
          m_inference(std::make_unique<Inference>(m_model_path)),
          m_pnp_solver(pnp_solver_ptr)
          {}

Detector::Detector(Detector&& detector) noexcept
        : m_model_path(detector.m_model_path),
          m_intrinsic_matrix(detector.m_intrinsic_matrix),
          m_distortion_vector(detector.m_distortion_vector),
          m_inference(std::move(detector.m_inference)) {}

Detector& Detector::operator=(Detector&& other)  noexcept {
    if (this != &other) {
        m_inference = std::move(other.m_inference);
        m_pnp_solver = std::move(other.m_pnp_solver);
    }
    return *this;
}

bool Detector::detect(const cv::Mat& frame, std::vector<Armor>* armors) {
    armors->clear();
    bool is_ok = false;

    std::vector<InferenceResult> inference_result;
    bool status = m_inference->inference(frame, &inference_result);
    if (status) {
        for (int i = 0; i < inference_result.size(); ++i) {
            if (static_cast<ArmorColor>(inference_result[i].color) != m_detect_color /*&&
                  static_cast<ArmorColor>(inference_result[i].color) != ArmorColor::GREY*/)
                continue;
            armors->emplace_back(armor_auto_aim::InferenceResult(inference_result[i]));
            is_ok = m_pnp_solver->obtain3dPose((*armors)[i]);
            Eigen::Vector3d c_point((*armors)[i].pose.x, (*armors)[i].pose.y, (*armors)[i].pose.z);
            if (!is_ok)
                armors->pop_back();
        }
        return !armors->empty();
    } else {
        return is_ok;
    }
}
} // armor_auto_aim