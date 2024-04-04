/**
 * @projectName armor_auto_aim
 * @file interface.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-29 19:03
 */

#ifndef ARMOR_AUTO_AIMING_SOLVER_INTERFACE_H
#define ARMOR_AUTO_AIMING_SOLVER_INTERFACE_H

#include <ostream>

#include <fmt/format.h>

namespace armor_auto_aim::solver {
    struct Pose {
        float pitch;
        float yaw;
        float roll;
        float x, y, z;

        Pose() =default;

        Pose(float p, float yaw, float r, float x, float y, float z)
            : pitch(p), yaw(yaw), roll(r), x(x), y(y), z(z)
        {}

        std::string to_string() const {
            return fmt::format("[Pose(xyz,ypr) => {}, {}, {}; {}, {}, {};]",
                               x, y, z, yaw, pitch, roll);
        }

        friend std::ostream& operator<<(std::ostream& os, const Pose& pose) {
            os << pose.to_string();
            return os;
        }
    };
};

#endif //ARMOR_AUTO_AIMING_SOLVER_INTERFACE_H
