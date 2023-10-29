/**
 * @projectName armor_auto_aiming
 * @file interface.cpp
 * @brief 接口
 * 
 * @author yx 
 * @date 2023-10-27 20:09
 */

#ifndef ARMOR_AUTO_AIMING_INTERFACE_H
#define ARMOR_AUTO_AIMING_INTERFACE_H

#include <opencv2/core.hpp>
#include <ostream>

namespace armor_auto_aiming {
    struct Armor {
        int area{};
        int color{};
        int classification{};
        float probability{};
        cv::Rect_<float> rect{};  // 装甲板的包围矩形
        cv::Point2f armor_apex[4];  // 装甲板的四个顶点
        std::vector<cv::Point2f> points{};

        Armor() =default;

        friend std::ostream& operator<<(std::ostream& os, const Armor& armor) {
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

    struct GridAndStride {
        int grid_x;
        int grid_y;
        int stride;  // 步长
    };
} // armor_auto_aiming

#endif //ARMOR_AUTO_AIMING_INTERFACE_H
