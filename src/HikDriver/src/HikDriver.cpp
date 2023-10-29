/**
 * @projectName IntelligentHanding
 * @file HikDriver.cpp
 * @brief 海康相机驱动
 *
 * @author yx
 * @date 2023-07-15 20:12
 */

#include <HikDriver/HikDriver.h>

std::string HikDriver::getParamInfo() const {
    std::string info = "Param Info: \n";
    info += "    Auto Exposure Time: " + getAutoExposureTime() + "\n";
    info += "    Auto Gain: " + getAutoGain() + "\n";
    auto _ = getExposureTime();
    info += "    Exposure Time: "
            "\n        current: " + std::to_string(_.current) +
            "\n        min: " + std::to_string(_.min) +
            "\n        max: " + std::to_string(_.max) + "\n";

    _ = getGain();
    info += "    Gain: "
            "\n        current: " + std::to_string(_.current) +
            "\n        min: " + std::to_string(_.min) +
            "\n        max: " + std::to_string(_.max) + "\n";

    return info;
}

void HikDriver::showParamInfo() const
{
    std::cout << "=================================\n";
    std::cout << getParamInfo() << std::endl;
    std::cout << "=================================\n";
}

bool HikDriver::enumDriver(int _mode)
{
    unsigned int status = MV_CC_EnumDevices(_mode, &m_devices);
    if (status != MV_OK) {
        logger.error("MV_CC_EnumDevices fail! status code: [0x{:02X}]\n", status);
        m_opened_status = false;
        return false;
    }
    if (m_devices.nDeviceNum <= 0) {
        logger.warn("Find No Devices!");
        m_opened_status = false;
        return false;
    }
    return true;
}

bool HikDriver::openDriver(int _index)
{
    QMutexLocker locker(&m_handle_lock);

    MV_CC_CreateHandle(&m_handle, m_devices.pDeviceInfo[_index]);
    MV_CC_OpenDevice(m_handle);
    MV_CC_SetEnumValue(m_handle, "TriggerMode", 0);
    unsigned int status = MV_CC_StartGrabbing(m_handle);
    if (!checkStatusCode(status, "openDriver Failed!"))
        return false;

    m_opened_status = true;
    return true;
}

bool HikDriver::initFrameInfo()
{
    if (!m_opened_status) {
        logger.error("driver no opened!");
        return false;
    }

    m_handle_lock.lock();
    MV_CC_GetIntValue(m_handle, "PayloadSize", &m_frame_size_info);
    m_handle_lock.unlock();

    m_data_size = m_frame_size_info.nCurValue;
    m_is_initialized_frame_info = true;

    return true;
}

bool HikDriver::readImageData(unsigned char* _data_buffer, MV_FRAME_OUT_INFO_EX& _frame_info)
{
    if (!m_is_initialized_frame_info) {
        logger.error("failed: frame info is not initialize!");
        return false;
    }

    QMutexLocker locker(&m_handle_lock);
    unsigned int status = MV_CC_GetOneFrameTimeout(m_handle, _data_buffer, m_data_size, &_frame_info, 1000);
    if (status != MV_OK)
        logger.error("No data[{:02X}]\n", status);

    return status == MV_OK;
}

void HikDriver::infoToString()
{
    for (const auto& driver_info: m_devices.pDeviceInfo) {
        if (driver_info == nullptr) {
            logger.error("failed: driver info is nullptr!");
        }
        std::string model_name(reinterpret_cast<char *>(driver_info->SpecialInfo.stGigEInfo.chModelName),
                               sizeof(driver_info->SpecialInfo.stGigEInfo.chModelName));
        std::string user_defined_name(reinterpret_cast<char *>(driver_info->SpecialInfo.stGigEInfo.chUserDefinedName),
                                      sizeof(driver_info->SpecialInfo.stGigEInfo.chUserDefinedName));

        if (driver_info->nTLayerType == MV_GIGE_DEVICE)  // GIGE 类型设备
        {
            unsigned int nIp1 = ((driver_info->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
            unsigned int nIp2 = ((driver_info->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
            unsigned int nIp3 = ((driver_info->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
            unsigned int nIp4 = (driver_info->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
            logger.info("Device Type GIGE");
            logger.info("Device Model Name: {}", model_name);
            logger.info("UserDefinedName: {}" , user_defined_name);
            logger.info("CurrentIp: {}.{}.{}.{}" , nIp1, nIp2, nIp3, nIp4);
        } else if (driver_info->nTLayerType == MV_USB_DEVICE) {  // USB 类设备
            logger.info("Device Type USB");
            logger.info("Device Model Name: {}", model_name);
            logger.info("UserDefinedName: {}", user_defined_name);
        } else {
            logger.info("Not support.");
        }
    }
}

void HikDriver::setExposureTimeRange(int lower, int upper)
{
    unsigned int code = MV_CC_SetIntValue(m_handle, "AutoExposureTimeLowerLimit", lower);
    checkStatusCode(code, "setExposureTimeRange() - lower");
    code = MV_CC_SetIntValue(m_handle, "AutoExposureTimeUpperLimit", upper);
    checkStatusCode(code, "setExposureTimeRange() - upper");
}

void HikDriver::setGainRange(int lower, int upper)
{
    unsigned int code = MV_CC_SetIntValue(m_handle, "AutoGainLowerLimit", lower);
    checkStatusCode(code, "setGainRange() - lower");
    code = MV_CC_SetIntValue(m_handle, "AutoGainUpperLimit", upper);
    checkStatusCode(code, "setGainRange() - upper");
}

void HikDriver::setAuto(int brightness)
{
    /* 自动曝光、自动增益、亮度255 */
    QMutexLocker locker(&m_handle_lock);
    MV_CC_SetEnumValue(m_handle, "ExposureAuto", 2);
    MV_CC_SetEnumValue(m_handle, "GainAuto", 2);
    unsigned int code = MV_CC_SetIntValue(m_handle, "Brightness", brightness);
    checkStatusCode(code, "setAuto()");
}

void HikDriver::setAutoExposureTime(int mode)
{
    QMutexLocker locker(&m_handle_lock);
    unsigned int code = MV_CC_SetEnumValue(m_handle, "ExposureAuto", mode);
    checkStatusCode(code, "setAutoExposureTime()");
}

void HikDriver::setAutoGain(int mode)
{
    QMutexLocker locker(&m_handle_lock);
    unsigned int code = MV_CC_SetEnumValue(m_handle, "GainAuto", mode);
    checkStatusCode(code, "setAutoGain()");
}

std::string HikDriver::getAutoExposureTime() const
{
    MVCC_ENUMVALUE r;
    unsigned int code = MV_CC_GetEnumValue(m_handle, "ExposureAuto", &r);
    if (code != MV_OK) {
        std::cout << "Failed: getIsAutoExposureTime()! -> " << std::hex << code << std::dec << std::endl;
        return "NULL";
    } else {
        switch (r.nCurValue) {
            case 0:
                return "Off";
            case 1:
                return "Once";
            case 2:
                return "Continuous";
            default:
                return "NULL";
        }
    }
}

std::string HikDriver::getAutoGain() const
{
    MVCC_ENUMVALUE r;
    unsigned int code = MV_CC_GetEnumValue(m_handle, "GainAuto", &r);
    if (code != MV_OK) {
        std::cout << "Failed: getIsAutoExposureTime()! -> " << std::hex << code << std::dec << std::endl;
        return "NULL";
    } else {
        switch (r.nCurValue) {
            case 0:
                return "Off";
            case 1:
                return "Once";
            case 2:
                return "Continuous";
            default:
                return "NULL";
        }
    }
}

void HikDriver::setExposureTime(float timems)
{
//    MV_CC_SetEnumValue(m_handle, "ExposureAuto", 0);
    unsigned int code = MV_CC_SetFloatValue(m_handle, "ExposureTime", timems);
    checkStatusCode(code, "setExposureTime()");
}

HikDriver::ParamInfo<float> HikDriver::getExposureTime() const
{
    MVCC_FLOATVALUE val;
    MV_CC_GetFloatValue(m_handle, "ExposureTime", &val);

    return HikDriver::ParamInfo<float>(val);
}

void HikDriver::setGain(float val)
{
    MV_CC_SetEnumValue(m_handle, "GainAuto", 0);
    unsigned int code = MV_CC_SetFloatValue(m_handle, "Gain", val);
    checkStatusCode(code, "setGain()");
}

HikDriver::ParamInfo<float> HikDriver::getGain() const
{
    MVCC_FLOATVALUE val;
    MV_CC_GetFloatValue(m_handle, "Gain", &val);

    return HikDriver::ParamInfo<float>(val);
}

void HikDriver::saveAccess(const std::string& user_file, const std::string& device_file)
{
    MV_CC_FILE_ACCESS file_access = {};
    file_access.pUserFileName = user_file.c_str();
    file_access.pDevFileName = device_file.c_str();

    unsigned int code = MV_CC_FileAccessWrite(m_handle, &file_access);
    checkStatusCode(code, "saveAccess()");
}
void HikDriver::loadAccess(const std::string& user_file, const std::string& device_file)
{
    MV_CC_FILE_ACCESS file_access = {};
    file_access.pUserFileName = user_file.c_str();
    file_access.pDevFileName = device_file.c_str();

    unsigned int code = MV_CC_FileAccessRead(m_handle, &file_access);
    checkStatusCode(code, "loadAccess()");
}
