/**
 * @projectName armor_auto_aim
 * @file interface.cpp
 * @brief 接口
 * 
 * @author yx 
 * @date 2023-10-27 20:09
 */

#ifndef ARMOR_AUTO_AIMING_INTERFACE_H
#define ARMOR_AUTO_AIMING_INTERFACE_H

#include <ostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <solver/solver_interface.h>

namespace armor_auto_aim {
    constexpr float k_ArmorTypeThreshold = 2.8f;  // 大于该阈值为大装甲板

    struct InferenceResult {
        int area{};  // 装甲板四个顶点所成四边形面积
        int color{};  // Blue: 0; Red: 1; 灰色: 2
        int classification{}; // 编号
        float probability{};
        cv::Rect_<float> rect{};  // 装甲板的包围矩形
        cv::Point2f armor_apex[4];  // 装甲板的四个顶点(左上角开始逆时针)
        std::vector<cv::Point2f> points{};  // 用来均值化的装甲板角点(iou > 0.9f)

        InferenceResult() =default;

        friend std::ostream& operator<<(std::ostream& os, const InferenceResult& armor) {
            os << "armor_apex: ";
            for (const auto & item : armor.armor_apex)
                os << item << "; ";
            os << "rect: " << armor.rect
               << "; classification: " << armor.classification
               << "; color: " << armor.color
               << "; area: " << armor.area
               << "; probability: "<< armor.probability
               << "; points: " << armor.points;
            return os;
        }
    };

    enum class ArmorColor { BLUE, RED, GREY, UNKNOWN };
    enum class ArmorType { LARGE, SMALL, UNKNOWN };

    struct Armor {
        ArmorColor color = ArmorColor::UNKNOWN;
        int number = 0;
        ArmorType type = ArmorType::UNKNOWN;
        std::vector<cv::Point2f> armor_apex;  // 装甲板的四个顶点(左上角开始逆时针)
        armor_auto_aim::solver::Pose pose{};

        Armor() =default;

        explicit Armor(const InferenceResult& inference_result)
            : number(inference_result.classification),
              color(static_cast<ArmorColor>(inference_result.color)),
              armor_apex(inference_result.armor_apex, inference_result.armor_apex + 4)
        {
            // TODO: 平衡步兵装甲板类型
            cv::RotatedRect r_rect = cv::minAreaRect(armor_apex);
            auto apex_wh_ratio = std::max(r_rect.size.height, r_rect.size.width) /
                                 std::min(r_rect.size.height, r_rect.size.width);
            type = apex_wh_ratio > k_ArmorTypeThreshold? ArmorType::LARGE: ArmorType::SMALL;
        }
    };

    using Armors = std::vector<Armor>;

    struct GridAndStride {
        int grid_x;
        int grid_y;
        int stride;  // 步长
    };
} // armor_auto_aim

#endif //ARMOR_AUTO_AIMING_INTERFACE_H
