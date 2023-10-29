/**
 * @projectName armor_auto_aiming
 * @file inference.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-27 20:05
 */

#include <opencv2/opencv.hpp>
#include <detector/inference.h>

namespace armor_auto_aiming {
void Inference::initModel(const std::string& model_path) {
    m_core.set_property(ov::cache_dir(m_CACHE_DIR));
    m_model = m_core.read_model(model_path);
    m_compiled_model = m_core.compile_model(m_model, m_DRIVER);
    m_infer_request = m_compiled_model.create_infer_request();

//    DLOG(INFO) << getInputAndOutputsInfo(*m_model);
    LOG(INFO) << fmt::format("({})Model loading completed!", model_path);
}

bool Inference::detect(const cv::Mat& src, std::vector<Armor>* armors) {
    // 预处理 (en: pretreatment)
    cv::Mat resized_img = armor_auto_aiming::inference_parser::scaledResize(src, &m_transformation_matrix);
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
    inference_parser::decodeOutputs(output_tensor, src_width, src_height, m_transformation_matrix, armors);

    return !armors->empty();;
}
} // armor_auto_aiming