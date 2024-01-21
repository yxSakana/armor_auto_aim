/**
 * @projectName armor_auto_aim
 * @file inference.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-27 20:05
 */

#include <opencv2/core.hpp>
#include <glog/logging.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <armor_detector/inference.h>
#include <armor_detector/parser.h>
#ifdef DEBUG
#include <armor_detector/utils.h>
#endif

namespace armor_auto_aim {
Inference::Inference() {
    auto available = m_core.get_available_devices();
    LOG(INFO) << fmt::format("Inference devices: {}", available);
    m_device = (std::find(available.begin(), available.end(), "GPU") != available.end()
               ) ? "GPU" : "CPU";
    LOG(INFO) << "Inference device: " << m_device;
}

Inference::Inference(const std::string& model_path)
        : m_inference_result_ptr(new float),
          m_MODEL_PATH(model_path) {
    auto available = m_core.get_available_devices();
    LOG(INFO) << fmt::format("Inference available devices: {}", available);
    m_device = (std::find(available.begin(), available.end(), "GPU") != available.end()
               ) ? "GPU" : "CPU";
    LOG(INFO) << "Inference device: " << m_device;
    initModel(m_MODEL_PATH);
}

Inference::Inference(const std::string& model_path, const std::string& driver)
        : m_inference_result_ptr(new float),
          m_MODEL_PATH(model_path),
          m_device(driver) {
    auto available = m_core.get_available_devices();
    LOG(INFO) << fmt::format("Inference available devices: {}", available);
    LOG(INFO) << "Inference device: " << m_device;
    initModel(m_MODEL_PATH);
}

void Inference::initModel(const std::string& model_path) {
    m_core.set_property(ov::cache_dir(m_CACHE_DIR));
    m_model = m_core.read_model(model_path);
    m_compiled_model = m_core.compile_model(m_model, m_device);
    m_infer_request = m_compiled_model.create_infer_request();
//    DLOG(INFO) << getInputAndOutputsInfo(*m_model);
    LOG(INFO) << fmt::format("({})Model loading completed!", model_path);
}

bool Inference::inference(const cv::Mat& src, std::vector<InferenceResult>* inference_armors) {
    m_inference_result_ptr = nullptr;
    // 预处理 (en: pretreatment)
    cv::Mat resized_img = armor_auto_aim::inference_parser::scaledResize(src, &m_transformation_matrix);
    cv::Mat img_32f;
    cv::Mat split_img[3];
    resized_img.convertTo(img_32f, CV_32F);
    cv::split(img_32f, split_img);
    // 设置输入张量以运行推理(en: Set input tensor to run inference)
    ov::Tensor input_tensor = m_infer_request.get_input_tensor();
    int offset = INPUT_WIDTH * INPUT_HEIGHT;
    for (int i = 0; i < 3; ++i)
        std::memcpy(input_tensor.data<float>() + offset * i, split_img[i].data, INPUT_WIDTH * INPUT_HEIGHT * sizeof(float));
    m_infer_request.set_input_tensor(input_tensor);
    m_infer_request.infer();
    // 解析推理结果(en: Analytic inference results)
    int src_width = src.cols;
    int src_height = src.cols;
    ov::Tensor output_tensor = m_infer_request.get_output_tensor();
    m_inference_result_ptr = output_tensor.data<float>();
    inference_parser::decodeOutputs(m_inference_result_ptr, src_width, src_height, m_transformation_matrix, inference_armors);

    return !inference_armors->empty();;
}
} // armor_auto_aim