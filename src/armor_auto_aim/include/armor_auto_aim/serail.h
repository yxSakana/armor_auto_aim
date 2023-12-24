/**
 * @project_name auto_aim
 * @file serail.h
 * @brief
 * @author yx
 * @date 2023-12-24 17:40:24
 */

#ifndef ARMOR_AUTO_AIM_ARMOR_AUTO_AIM_SERIAL_H
#define ARMOR_AUTO_AIM_ARMOR_AUTO_AIM_SERIAL_H

#include <QObject>

#include <serial_port/VCOMCOMM.h>
#include <serial_port/communicate_protocol.h>
#include <google_logger/google_logger.h>

namespace armor_auto_aim {
class SerialWork: public QObject {
    Q_OBJECT
    using ProgramClock = std::chrono::system_clock;
    using ClockUnit = std::chrono::milliseconds;
public:
    explicit SerialWork(QObject* parent = nullptr);
public slots:
    void sendAimInfo(const AutoAimInfo& aim_info);
signals:
    void readyImuData(const ImuData& imu_data);
private:
    static constexpr uint16_t m_PcId = 0; // id: PC
    static constexpr uint8_t m_SendAutoAimInfoCode = 1; // code: 发送自瞄信息
    static constexpr uint8_t m_SendTimestampCode = 2; // code: 发送pc当前时间戳
    static constexpr uint16_t m_MicrocontrollerId = 1;  // id: 单片机
    static constexpr uint8_t m_ImuInfoCode = 0;  // code: IMU数据

    VCOMCOMM m_serial;

    void sendNowTimestamp();
private:
    void selectFunction(uint8_t code, uint16_t id, const QByteArray& data);
};
}
#endif //ARMOR_AUTO_AIM_ARMOR_AUTO_AIM_SERIAL_H
