/**
 * @project_name armor_auto_aiming
 * @file communication_protocol.h
 * @brief
 * @author yx
 * @data 2023-11-14 19:56:22
 */

#ifndef ARMOR_AUTO_AIMING_COMMUNICATION_PROTOCOL_H
#define ARMOR_AUTO_AIMING_COMMUNICATION_PROTOCOL_H

#include <cstdint>

#include <iostream>
#include <string>

struct PredictData {
    float yaw{};
    float pitch{};
    float distance{};
    uint8_t aim_shoot{};

    PredictData() =default;

    PredictData(const float& y, const float& p, const float& d)
        : yaw(y),
          pitch(p),
          distance(d) {}

    [[nodiscard]] std::string to_string() const {
        std::string info("[PredictData ");
        info += "yaw: " + std::to_string(yaw) + "; ";
        info += "pitch: " + std::to_string(pitch) + "; ";
        info += "distance: " + std::to_string(distance) + "; ";
        info += "aim_shoot: " + std::to_string(aim_shoot) + ";]";
        return info;
    }
};

struct ImuData {
    struct ImuQuaternion {
        float w{};
        float x{};
        float y{};
        float z{};

        std::string to_string() const {
            std::string info("[ImuQuaternion ");
            info += std::to_string(w) + ", " +
                    std::to_string(x) + ", " +
                    std::to_string(y) + ", " +
                    std::to_string(z);
            info += "(wxyz)]";
            return info;
        }
    } quaternion{};

    std::string to_string() const {
        std::string info("[ImuData ");
        info += quaternion.to_string();
        info += "]";
        return info;
    }
};

#endif //ARMOR_AUTO_AIMING_COMMUNICATION_PROTOCOL_H
