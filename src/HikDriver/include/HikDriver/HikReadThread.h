/**
 * @projectName IntelligentHanding
 * @file HikReadThread.h
 * @brief 读取相机帧
 *
 * @author yx
 * @date 2023-07-18 20:12
 */

#ifndef INTELLIGENTHANDING_HIKREADTHREAD_H
#define INTELLIGENTHANDING_HIKREADTHREAD_H

#include <QThread>
#include <QReadLocker>
#include <QReadWriteLock>

#include <spdlog_factory/spdlogger.h>
#include <HikDriver/HikDriver.h>
#include <HikDriver/HikFrame.hpp>

class HikReadThread : public QThread {
    Q_OBJECT
public:
    HikReadThread()
        : QThread(nullptr),
          logger(__FUNCTION__)
    {}

    explicit HikReadThread(HikDriver* _driver)
            : QThread(nullptr),
              m_data_size(_driver->getDadaSize()),
              m_buffer(new unsigned char[m_data_size]),
//              m_frame(m_data_size),
              logger(__FUNCTION__)
    {
        m_driver = _driver;
    }

    ~HikReadThread() override {
        delete[] m_buffer;
    }

    void setDriver(HikDriver* _driver) {
        m_data_size = _driver->getDadaSize();
        m_buffer = new unsigned char[m_data_size];
    }

    HikFrame getFrame() {
        QReadLocker r_locker(&m_frame_lock);
        return m_frame;
    }

    cv::Mat getRgbMat() {
        QReadLocker r_locker(&m_frame_lock);
        return m_frame.m_rgb_frame;
    }

    void run() override;
signals:
    void readyData(const HikFrame& frame);
private:
    unsigned int m_data_size = {};  // 帧大小
    unsigned char* m_buffer = {};  // 缓冲区
    MV_FRAME_OUT_INFO_EX m_frame_info = {};
    HikFrame m_frame;
    QReadWriteLock m_frame_lock;
    HikDriver* m_driver{};

    spdlogger logger;
};


#endif //INTELLIGENTHANDING_HIKREADTHREAD_H
