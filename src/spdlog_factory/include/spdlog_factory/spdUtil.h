/**
 * @file spdUtil.h
 * @author yao
 * @date 2021年1月21日
 * @brief spdlog工具包
 */

#ifndef KDROBOTCPPLIBS_SPDUTIL_H
#define KDROBOTCPPLIBS_SPDUTIL_H

#include <string>

namespace spdUtil {

    /**
     * 将字节为单位的容量转换为带单位的字符串,单位自动选择,最大TB,保留一位小数
     * @param size 大小
     * @return 转换的字符串
     */
    std::string formatSize(size_t size);
}


#endif //KDROBOTCPPLIBS_SPDUTIL_H
