/**
 * @projectName armor_auto_aim
 * @file test_utils.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-27 21:50
 */

#ifndef ARMOR_AUTO_AIMING_UTILS_H
#define ARMOR_AUTO_AIMING_UTILS_H

#include <string>

#include <openvino/openvino.hpp>

namespace armor_auto_aim {
/**
 * @brief 获取模型信息(input|output name、type、shape) | (en: Get model information)
 *
 * @param network
 */
std::string getInputAndOutputsInfo(const ov::Model& network);
}

#endif //ARMOR_AUTO_AIMING_UTILS_H
