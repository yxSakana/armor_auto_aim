/**
 * @projectName IntelligentHanding
 * @file HikReadThread.cpp
 * @brief 读取相机帧
 *
 * @author yx
 * @date 2023-07-18 20:12
 */

#include <HikDriver/HikReadThread.h>

void HikReadThread::run() {
    if (m_data_size <= 0) {
        logger.error("Failed: data size {}", m_data_size);
        return;
    }

    logger.info("Hik Frame Stream Starting...");
    while (true) {
        bool is_ok = m_driver->readImageData(m_buffer, m_frame_info);
        if (is_ok) {
            cv::Mat frame(m_frame_info.nHeight, m_frame_info.nWidth, CV_8UC1, m_buffer);
            m_frame_lock.lockForWrite();
            cv::cvtColor(frame, m_frame.m_rgb_frame, cv::COLOR_BayerRG2RGB);
            if (m_frame.m_rgb_frame.empty())
                logger.error("Failed: frame empty!");
            m_frame_lock.unlock();
            m_frame.m_timestamp = m_frame_info.nHostTimeStamp;
            emit readyData(m_frame);
        } else {
            logger.error("Failed: No data!");
        }
    }

}
