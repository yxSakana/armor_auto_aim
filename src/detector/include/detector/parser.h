/**
 * @projectName armor_auto_aiming
 * @file parser.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-27 19:57
 */

#ifndef ARMOR_AUTO_AIMING_PARSER_H
#define ARMOR_AUTO_AIMING_PARSER_H

#include <thread>
#include <algorithm>

#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <detector/inference.h>

namespace armor_auto_aiming::inference_parser {
struct GridAndStride {
    int grid0;
    int grid1;
    int stride;
};

static constexpr int k_NumberClassification = 8;
static constexpr int k_NumberColor = 4;
static constexpr int k_InferenceResultPollMaxVal = 128;  // 最多保留的推理出的装甲板数量(保留概率更大的)
static constexpr float k_BoundBoxConfidenceThreshold = 0.75f;  // 推理输出的边框的概率的阈值(en: bounding box confidence threshold)
static constexpr float k_NmsThreshold = 0.35f;  // 非极大值抑制-阈值(en: Non-Maximum Suppression)
static constexpr float k_MergeConfidenceError = 0.15f;  // 重合置信度误差
static constexpr float k_UseToMeanIouThreshold = 0.9f;  // 高于该阈值的iou可以被用来均值化装甲板的四个顶点以减少误差

/**
 * @brief 调整图像并设置变换矩阵
 *
 * @param src[in]
 * @param transformation_matrix[out] 变换矩阵
 * @return 调整大小之后的图像
 */
cv::Mat scaledResize(const cv::Mat& src, Eigen::Matrix<float, 3, 3>* transformation_matrix);

/**
 * @brief 解析推理结果
 *
 * @param tensor[in] 推理结果
 * @param img_w[in] 原始src的宽高
 * @param img_h[in]
 * @param transform_matrix[in] 变换矩阵
 * @param armors[out] 装甲板对象
 */
void decodeOutputs(const ov::Tensor& tensor, const int& img_w, const int& img_h, const Eigen::Matrix<float, 3, 3>& transform_matrix,
                   std::vector<Armor>* armors);

/**
 * @brief 生成用来解析的网格和步长
 *
 * @param target_w[in] 模型的输入宽高
 * @param target_h[in]
 * @param strides[in] 步长
 * @param grid_strides[out] 结果
 */
void generateGridsAndStride(const int& target_w, const int& target_h, const std::vector<int>& strides,
                            std::vector<GridAndStride>* grid_strides);

/**
 * @brief 生成 YOLOX 解析结果
 *
 * @param inference_result_ptr[in] 推理结果的指针
 * @param grid_strides[in]
 * @param transform_matrix[in]
 * @param probability_confidence_threshold[in] 推理输出的边框的概率的阈值
 * @param armors[out]
 */
void generateYoloxProposals(const float* inference_result_ptr, const std::vector<GridAndStride>& grid_strides,
                            const Eigen::Matrix<float, 3, 3>& transform_matrix, const float& probability_confidence_threshold,
                            std::vector<Armor>* armors);

void nonMaximumSuppression(std::vector<Armor>& selected_armors, std::vector<Armor>& armors, const float& threshold);

/**
 * @brief 快速排序-降序
 */
void quicksort(std::vector<Armor>& armors, const int& left, const int& right);

float quadrilateralArea(cv::Point2f points[4]);
}

#endif //ARMOR_AUTO_AIMING_PARSER_H
