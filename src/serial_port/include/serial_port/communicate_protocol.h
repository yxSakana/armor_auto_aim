/**
 * @project_name armor_auto_aiming
 * @file communication_protocol.h
 * @brief
 * @author yx
 * @date 2023-11-14 19:56:22
 */

#ifndef ARMOR_AUTO_AIMING_COMMUNICATION_PROTOCOL_H
#define ARMOR_AUTO_AIMING_COMMUNICATION_PROTOCOL_H

#include <cstdint>

#include <iostream>
#include <string>
#include <chrono>

struct AutoAimInfo {
    float yaw{};
    float pitch{};
    float distance{};
    uint8_t is_shoot{};
    uint8_t tracker_status{};
    uint8_t data_id{};
#ifdef SERIAL
    uint8_t id{};
#endif

    AutoAimInfo() =default;

    AutoAimInfo(const float& y, const float& p, const float& d)
        : yaw(y),
          pitch(p),
          distance(d) {}

    [[nodiscard]] std::string to_string() const {
        std::string info("[AutoAimInfo ");
#ifdef SENTRY
        info += "id: " + std::to_string(id) + "; ";
#endif
        info += "yaw: " + std::to_string(yaw) + "; ";
        info += "pitch: " + std::to_string(pitch) + "; ";
        info += "distance: " + std::to_string(distance) + "; ";
        info += "is_shoot: " + std::to_string(is_shoot) + "; ";
        info += "tracker_status: " + std::to_string(tracker_status) + ";]";
        return info;
    }

    void reset() {
        yaw = pitch = distance = is_shoot = tracker_status = 0;
    }
};

struct ImuData {
    uint64_t timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    uint8_t data_id{};
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
        info += "timestamp: " + std::to_string(timestamp) + ", ";
        info += "data_id: " + std::to_string(data_id) + ", ";
        info += quaternion.to_string();
        info += "]";
        return info;
    }
};

#endif //ARMOR_AUTO_AIMING_COMMUNICATION_PROTOCOL_H
