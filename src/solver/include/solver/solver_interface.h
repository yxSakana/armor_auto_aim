/**
 * @projectName armor_auto_aiming
 * @file interface.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-29 19:03
 */

#ifndef ARMOR_AUTO_AIMING_SOLVER_INTERFACE_H
#define ARMOR_AUTO_AIMING_SOLVER_INTERFACE_H

#include <ostream>

namespace armor_auto_aiming::solver {
    struct SpatialLocation {
        float pitch;
        float yaw;
        float roll;
        float x, y, z;

        SpatialLocation() =default;

        SpatialLocation(float p, float yaw, float r, float x, float y, float z)
            : pitch(p), yaw(yaw), roll(r), x(x), y(y), z(z)
        {}

        friend std::ostream& operator<<(std::ostream& os, const SpatialLocation& location) {
            os << "pitch: " << location.pitch
               << "; yaw: " << location.yaw
               << "; roll: " << location.roll
               << "; x: " << location.x
               << "; y: " << location.y
               << "; z: " << location.z;
            return os;
        }
    };
};

#endif //ARMOR_AUTO_AIMING_SOLVER_INTERFACE_H
