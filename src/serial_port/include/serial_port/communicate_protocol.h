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

struct CommunicateProtocol {
    float yaw{};
    float pitch{};
    float distance{};

    CommunicateProtocol() =default;

    CommunicateProtocol(const float& y, const float& p, const float& d)
        : yaw(y),
          pitch(p),
          distance(d) {}
};

#endif //ARMOR_AUTO_AIMING_COMMUNICATION_PROTOCOL_H
