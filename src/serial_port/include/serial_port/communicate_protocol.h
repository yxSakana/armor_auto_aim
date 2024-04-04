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

#include <fmt/format.h>

struct AutoAimInfo {
    float x = .0f, y = .0f, z = 0.0f;
    float v_x = .0f, v_y = .0f, v_z = .0f;
    float theta = .0f;
    float omega = .0f;
    float r = .0f;
    uint8_t delay = .0f;  // 视觉程序延迟
    uint8_t tracker_status{};
#ifdef SERIAL
    uint8_t id{};
#endif

    AutoAimInfo() =default;

    AutoAimInfo(const float x, const float y, const float z,
                const float v_x, const float v_y, const float v_z,
                const float theta, const float w, const float r,
                const float delay, const uint8_t status)
            : x(x), y(y), z(z), v_x(v_x), v_y(v_y), v_z(v_z),
              theta(theta), omega(w), r(r), delay(delay),
              tracker_status(status) {}

    [[nodiscard]] std::string to_string() const {
        return fmt::format(
                "[AutoAimInfo => d: ({}, {}, {}); v: ({}, {}, {}); "
                "theta: {}; omega: {}; r: {}; delay: {}; "
                "tracker_status: {};]",
                x, y, z, v_x, v_y, v_z,
                theta, omega, r,
                delay, tracker_status);
#ifdef SENTRY
        return fmt::format(
                "[AutoAimInfo => d: ({}, {}, {}); v: ({}, {}, {}); "
                "r: {}; w: {}; delay: {}; "
                "is_shoot: {}; tracker_status: {}; data_id: {}; id: {}]",
                x, y, z, v_x, v_y, v_z, r, w, delay,
                is_shoot, tracker_status, data_id, id);
#endif
    }

    void reset() {
        x = y = z =
        v_x = v_y = v_z =
        theta = omega = r = delay =
        tracker_status = 0;
    }
};

struct ImuData {
    uint64_t timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
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
    float yaw, pitch, roll;

    std::string to_string() const {
        std::string info("[ImuData ");
        info += "timestamp: " + std::to_string(timestamp) + ", ";
        info += quaternion.to_string();
        info += "]";
        return info;
    }
};

struct AimPoint {
    float x, y, z;
};

#endif //ARMOR_AUTO_AIMING_COMMUNICATION_PROTOCOL_H
