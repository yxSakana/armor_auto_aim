/**
 * @projectName IntelligentHanding
 * @file HikDriver.h
 * @brief 海康相机驱动
 *
 * @author yx
 * @date 2023-07-15 20:12
 */

#ifndef HIKDRIVER_HIKDRIVER_H
#define HIKDRIVER_HIKDRIVER_H

#include <cstring>
#include <unistd.h>

#include <string>

#include <QMutex>
#include <opencv2/opencv.hpp>
#include <MvCameraControl.h>

#include <spdlog_factory/spdlogger.h>

class HikDriver {
public:
    template<typename T>
    struct ParamInfo {
        T current;
        T min;
        T max;

        explicit ParamInfo(MVCC_FLOATVALUE & p)
        {
            current = p.fCurValue;
            min = p.fMin;
            max = p.fMax;
        }

        [[nodiscard]] std::string toString() const
        {
            std::string info;
            info = "current: " + std::to_string(current) +
                    "\nmin: " + std::to_string(min) +
                    "\nmax: " + std::to_string(max);
            return info;
        }
    };

    HikDriver()
        : logger(__FUNCTION__),
          m_devices{}
    {}

    /**
     * @param _index 相机索引
     */
    explicit HikDriver(int _index)
        : logger(__FUNCTION__),
          m_devices{}
    {
        connectDriver(_index);
    }

    ~HikDriver() {
        MV_CC_StopGrabbing(m_handle);  // TODO: ??
        MV_CC_CloseDevice(m_handle);
        MV_CC_DestroyHandle(m_handle);
    }

    /**
     * 获取相机的所有参数
     *
     * @return 参数组成的字符串
     */
    [[nodiscard]] std::string getParamInfo() const;

    /**
     * 显示所有相机参数
     */
    void showParamInfo() const;

    /**
     * 枚举所有设备
     *
     * @param _mode
     * @return
     */
    bool enumDriver(int _mode=MV_GIGE_DEVICE | MV_USB_DEVICE);

    /**
     * 重新加载帧信息
     */
    void reloadFrameInfo() {
        m_is_initialized_frame_info = false;
        this->initFrameInfo();
    }

    /**
     * 连接相机. 打开相机，并初始化帧信息
     *
     * @param _index 索引
     * @return
     */
    bool connectDriver(int _index=0) {
        return this->enumDriver() && this->openDriver(_index) && this->initFrameInfo();
    }

    /**
     * 读取一帧图像
     *
     * @param _src
     */
    bool readImageData(unsigned char* _data_buffer, MV_FRAME_OUT_INFO_EX& _frame_info);

    /**
     * 设置自动曝光时间的范围
     *
     * @param lower
     * @param upper
     */
    void setExposureTimeRange(int lower, int upper);

    /**
     * 设置自动增益时间的范围
     *
     * @param lower
     * @param upper
     */
    void setGainRange(int lower, int upper);

    /**
     * 设置自动曝光、自动增益
     */
    void setAuto(int brightness=255);

    /**
     * 自动曝光 type
     * @return
     */
     void setAutoExposureTime(int mode);
    [[nodiscard]] std::string getAutoExposureTime() const;

    /**
     * 自动增益 type
     * @return
     */
     void setAutoGain(int mode);

    [[nodiscard]] std::string getAutoGain() const;

    /**
     * 设置曝光时间(自动将自动曝光关闭)
     *
     * @param timems
     */
    void setExposureTime(float timems);

    [[nodiscard]] ParamInfo<float> getExposureTime() const;

    /**
     * 设置增益值(自动将自动增益关闭)
     *
     * @param val
     */
    void setGain(float val);

    [[nodiscard]] HikDriver::ParamInfo<float> getGain() const;

    [[nodiscard]] unsigned int getDadaSize() const { return m_data_size; }

    [[nodiscard]] bool isConnected() const { return m_opened_status && m_is_initialized_frame_info; }

    /**
     * 保存/加载配置
     *
     * @param user_file 用户文件
     * @param device_file 设备文件
     */
    void saveAccess(const std::string& user_file, const std::string& device_file);

    void loadAccess(const std::string& user_file, const std::string& device_file);

    /**
     * -*-
     */
    void infoToString();

private:
    void* m_handle = nullptr; // 设备句柄
    QMutex m_handle_lock;

    MV_CC_DEVICE_INFO_LIST m_devices;  // 所有设备
    bool m_opened_status = false;  // 打开状态

    bool m_is_initialized_frame_info = false;  // 是否已经初始化完成信息

    MVCC_INTVALUE m_frame_size_info = {};  // 帧大小信息
    unsigned int m_data_size = 0;  // 帧大小

    spdlogger logger;

    /**
     * 打开设备
     * @param _index 索引
     * @return
     */
    bool openDriver(int _index=0);

    /**
     * 初始化帧信息
     * @return
     */
    bool initFrameInfo();

    inline bool checkStatusCode(const unsigned int code, const std::string& mess="") {
        if (code != MV_OK) {
            logger.error("{} status code: [0x{:02x}]\n", mess, code);
            return false;
        } else
            return true;
    }
};


#endif //HIKDRIVER_HIKDRIVER_H
