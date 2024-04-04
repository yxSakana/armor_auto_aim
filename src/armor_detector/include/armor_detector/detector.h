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
    Detector() =default;

    Detector(const std::string& model_filename,
             const std::array<double, 9>& intrinsic_matrix,
             const std::vector<double>& distortion_cess);

    Detector(const std::string& model_filename,
             std::shared_ptr<PnPSolver> pnp_solver_ptr);

    Detector(Detector&& detector) noexcept;

    Detector& operator=(Detector&& other) noexcept;

    void setDetectColor(const ArmorColor& color) { m_detect_color = color; }

    bool detect(const cv::Mat& frame, std::vector<Armor>* armors);
private:
    const std::string m_model_path{};
    const std::array<double, 9> m_intrinsic_matrix{};
    const std::vector<double> m_distortion_vector{};

    ArmorColor m_detect_color;
    std::unique_ptr<Inference> m_inference;
    std::shared_ptr<PnPSolver> m_pnp_solver;
};

} // armor_auto_aim

#endif //ARMOR_AUTO_AIMING_DETECTOR_H
