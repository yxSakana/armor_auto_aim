/**
 * @projectName armor_auto_aiming
 * @file inference.cpp
 * @brief 推理
 * 
 * @author yx 
 * @date 2023-10-27 20:05
 */

#ifndef ARMOR_AUTO_AIMING_INFERENCE_H
#define ARMOR_AUTO_AIMING_INFERENCE_H

#include <string>

#include <Eigen/Dense>
#include <openvino/openvino.hpp>

#include <detector/interface.h>

namespace armor_auto_aiming {

class Inference {
public:
    Inference() =default;

    explicit Inference(const std::string& model_path) { initModel(model_path); }

    void initModel(const std::string& model_path);

    bool inference(const cv::Mat& src, std::vector<InferenceResult>* inference_armors);

    static constexpr int INPUT_WIDTH = 416;
    static constexpr int INPUT_HEIGHT = 416;
private:
    ov::Core m_core;
    std::shared_ptr<ov::Model> m_model;
    ov::CompiledModel m_compiled_model;
    ov::InferRequest m_infer_request;
    Eigen::Matrix<float, 3, 3> m_transformation_matrix;

    const std::string m_MODEL_PATH = "../../model/opt-0527-001.xml";
    const std::string m_DRIVER = "CPU";
    const std::string m_CACHE_DIR = "../.cache";
};

} // armor_auto_aiming

#endif //ARMOR_AUTO_AIMING_INFERENCE_H
