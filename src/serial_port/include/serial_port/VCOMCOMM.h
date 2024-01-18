/**
 * @file VCOMCOMM.h
 * @brief 虚拟串口通信
 * @author yao
 * @date 2021年1月13日
 */

#ifndef KDROBOTCPPLIBS_VCOMCOMM_PC_H
#define KDROBOTCPPLIBS_VCOMCOMM_PC_H

#include <QMetaEnum>
#include <QTimer>
#include <QThread>
#include <QSerialPort>

#include <serial_port/CRC.h>

namespace armor_auto_aim {
/**
 * @brief 虚拟串口
 * @details 可基于PID和VID或制造商名称自动搜索串口
 *          使用VCOMCOMM协议进行通信
 *          基于信号量可跨线程通信
 *          在每次发送数据时都会检测串口连接状态, 在断开时自动尝试连接再发送
 * @class VCOMCOMM
 */
class VCOMCOMM : public QSerialPort {
Q_OBJECT
public:
    /**
     * @brief 构造函数,构造时自动搜索对应PID和VID的USB串口设备
     * @param PID 产品ID
     * @param VID 制造商ID
     */
    VCOMCOMM(uint16_t PID = 22336, uint16_t VID = 1155, QObject* parent = nullptr);

    /**
     * @brief 构造函数,构造时自动搜索对应制造商名称的USB串口设备
     * @param manufacturer 制造商名称
     */
    VCOMCOMM(const QString& manufacturer, QObject* parent = nullptr);

    /**
     * @brief 自动连接对应制造商名称或PID和VID的USB串口设备
     *        优先搜索制造商名称
     * @return 自动连接是否成功
     */
    bool auto_connect();

    /**
     * @brief 设置PID和VID
     * @param PID 产品ID
     * @param VID 制造商ID
     */
    void setPidVid(uint16_t PID, uint16_t VID);

    /**
     * @brief 设置制造商名称
     * @param manufacturer 制造商名称
     */
    void setManufacturer(const QString& manufacturer);
protected slots:

    void portReadyRead();

    void portErrorOccurred(QSerialPort::SerialPortError error);
public slots:

    /**
     * @brief 发送消息
     * @param fun_code 功能码
     * @param id 消息ID
     * @param data 数据
     */
    void Transmit(uint8_t fun_code, uint16_t id, const QByteArray& data);
signals:
    /**
     * @brief 收到数据包信号量
     * @param fun_code 数据包功能码
     * @param id 数据包ID
     * @param data 数据
     */
    void receiveData(uint8_t fun_code, uint16_t id, const QByteArray& data);

    /**
     * @brief 通过信号量实现跨线程发送
     * @param fun_code 功能码
     * @param id 消息ID
     * @param data 数据
     */
    void CrossThreadTransmitSignal(uint8_t fun_code, uint16_t id, const QByteArray& data);
private:
    uint16_t pid, vid;
    QString manufacturer;
    Qt::HANDLE thread_id;
    QTimer* m_timer;
};
}
#endif
