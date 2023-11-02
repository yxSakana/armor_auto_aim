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
//    unsigned int m_data_size = {};  // 帧大小
    cv::Mat m_rgb_frame = {};
    int64_t m_timestamp = 0;

public:
//    HikFrame() =default;
    explicit HikFrame(/*const unsigned int& _data_size*/)
//        : m_data_size(_data_size)
    {}

    HikFrame(const HikFrame& other)
        : /*m_data_size(other.m_data_size),*/
          m_timestamp(other.m_timestamp)
    {}

    HikFrame(HikFrame&& other) noexcept
        : /*m_data_size(other.m_data_size),*/
          m_rgb_frame(std::move(other.m_rgb_frame)),
          m_timestamp(other.m_timestamp)
    {
//        other.m_data_size = 0;
        other.m_timestamp = 0;
    }

    ~HikFrame() =default;

    [[nodiscard]] inline cv::Mat getRgbFrame() const { return m_rgb_frame; }

    [[nodiscard]] inline int64_t getTimestamp() const { return m_timestamp; }
};


#endif //INTELLIGENTHANDING_HIKFRAME_HPP
