/**
 * @file spdUtil.cpp
 * @author yao
 * @date 2021年1月21日
 */

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <spdlog_factory/spdUtil.h>

namespace spdUtil {
    std::string formatSize(size_t size) {
        char buf[20];
        int len;
        float kb = size / 1024.0f;
        float mb = kb / 1024.0f;
        float gb = mb / 1024.0f;
        float tb = gb / 1024.0f;
        if (size < 1024) {
            len = sprintf(buf, "%4zuB", size);
        } else if (kb < 1024) {
            len = sprintf(buf, "%.1fKB", kb);
        } else if (mb < 1024) {
            len = sprintf(buf, "%.1fMB", mb);
        } else if (gb < 1024) {
            len = sprintf(buf, "%.1fGB", gb);
        } else {
            len = sprintf(buf, "%.1fTB", tb);
        }
        return std::string(buf, len);
    }
}
