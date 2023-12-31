/**
 * @projectName IntelligentHanding
 * @file HikFrame.hpp
 * @brief 海康相机帧对象
 *
 * 对象由当前帧的图像和时间戳组成
 *
 * @author yx
 * @date 2023-07-18 20:12
 */

#ifndef INTELLIGENTHANDING_HIKFRAME_HPP
#define INTELLIGENTHANDING_HIKFRAME_HPP

#include <opencv2/opencv.hpp>
#include <spdlog_factory/spdlogger.h>

class HikFrame {
    friend class HikReadThread;
private:
    cv::Mat m_rgb_frame = {};
    int64_t m_timestamp = 0;  // milliseconds

public:
    HikFrame() =default;

    HikFrame(const HikFrame& other)
        : m_timestamp(other.m_timestamp),
          m_rgb_frame(other.m_rgb_frame.clone())
    {}

    HikFrame(HikFrame&& other) noexcept
        : m_rgb_frame(std::move(other.m_rgb_frame)),
          m_timestamp(other.m_timestamp)
    {}

    ~HikFrame() =default;

    HikFrame& operator=(const HikFrame& other) {
        if (this != &other) {
            m_timestamp = other.m_timestamp;
            m_rgb_frame = other.m_rgb_frame.clone();
        }
        return *this;
    }

    HikFrame& operator=(HikFrame&& other) noexcept {
        if (this != &other) {
            m_timestamp = other.m_timestamp;
            m_rgb_frame = std::move(other.m_rgb_frame);
        }
        return *this;
    }

    [[nodiscard]] inline cv::Mat getRgbFrame() const { return m_rgb_frame; }

    [[nodiscard]] inline int64_t getTimestamp() const { return m_timestamp; }
};


#endif //INTELLIGENTHANDING_HIKFRAME_HPP
