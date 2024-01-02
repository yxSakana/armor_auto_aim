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

#include <Eigen/Dense>
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

    static std::string to_string(const ArmorColor& armor_color) {
        switch (armor_color) {
            case ArmorColor::BLUE: return "BLUE";
            case ArmorColor::RED: return "RED";
            case ArmorColor::GREY: return "GREY";
            case ArmorColor::UNKNOWN: return "UNKNOWN";
        }
        return "UNKNOWN";
    }

    static std::string to_string(const ArmorType& armor_type) {
        switch (armor_type) {
            case ArmorType::LARGE: return "LARGE";
            case ArmorType::SMALL: return "SMALL";
            case ArmorType::UNKNOWN: return "UNKNOWN";
        }
        return "UNKNOWN";
    }

    static std::string to_string(const std::vector<cv::Point2f>& point2f) {
        std::string result("[");
        for (int i = 0; i < point2f.size(); ++i) {
            result += fmt::format("({}, {})", point2f[i].x, point2f[i].y);
            if (i != point2f.size() - 1) result += ", ";
        }
        result += "]";
        return result;
    }

    struct Armor {
        ArmorColor color = ArmorColor::UNKNOWN;
        int number = 0;
        ArmorType type = ArmorType::UNKNOWN;
        std::vector<cv::Point2f> armor_apex;  // 装甲板的四个顶点(左上角开始逆时针)
        armor_auto_aim::solver::Pose pose{};
        Eigen::Vector3d world_coordinate;  // (imu参考的)惯性坐标系
        float probability{};

        Armor() =default;

        explicit Armor(const InferenceResult& inference_result)
            : number(inference_result.classification),
              color(static_cast<ArmorColor>(inference_result.color)),
              armor_apex(inference_result.armor_apex, inference_result.armor_apex + 4),
              probability(inference_result.probability)
        {
            // TODO: 平衡步兵装甲板类型
            cv::RotatedRect r_rect = cv::minAreaRect(armor_apex);
            auto apex_wh_ratio = std::max(r_rect.size.height, r_rect.size.width) /
                                 std::min(r_rect.size.height, r_rect.size.width);
            type = apex_wh_ratio > k_ArmorTypeThreshold? ArmorType::LARGE: ArmorType::SMALL;
        }

        std::string to_string() const {
            return fmt::format("[Armor->probability: {}; color: {}; number: {}; type: {}; armor_apes: {}; pose: {};]",
                               probability, armor_auto_aim::to_string(color), number,
                               armor_auto_aim::to_string(type), armor_auto_aim::to_string(armor_apex),
                               pose.to_string());  // TODO:
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
