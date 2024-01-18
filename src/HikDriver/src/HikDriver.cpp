/**
 * @projectName IntelligentHanding
 * @file HikDriver.cpp
 * @brief 海康相机驱动
 *
 * @author yx
 * @date 2023-07-15 20:12
 */

#include <glog/logging.h>
#include <fmt/format.h>

#include <HikDriver/HikDriver.h>

HikDriver::HikDriver()
        : m_devices{} {
    connectDriver();
}

HikDriver::HikDriver(int index)
        : m_index(index),
          m_devices{} {
    connectDriver();
}

HikDriver::HikDriver(int index, const TriggerSource& trigger)
        : m_index(index),
          m_trigger_source(trigger),
          m_is_trigger(true),
          m_devices{} {
    connectDriver();
}

HikDriver::~HikDriver() {
    MV_CC_StopGrabbing(m_handle);
    MV_CC_CloseDevice(m_handle);
    MV_CC_DestroyHandle(m_handle);
}

void HikDriver::reloadFrameInfo() {
    m_initialized_frame_info = false;
    this->initFrameInfo();
}

std::string HikDriver::getParamInfo() const {
    using namespace fmt::literals;
    auto v_et = getExposureTime();
    auto v_g = getGain();
    std::string info = fmt::format(
            "Param Info: index: {index}; {driver_info}\n"
            "    Trigger Mode: {trigger_mode}\n"
            "    Trigger Source: {trigger_source}\n"
            "    Trigger Delay: {trigger_delay}\n"
            "    Auto Exposure Time: {auto_exposure_time}\n"
            "    Auto Gain: {auto_gain}\n"
            "    Exposure Time:\n"
            "        current: {et_curr}\n"
            "        min: {et_min}\n"
            "        max: {et_max}\n"
            "    Gain: \n"
            "        current: {gain_curr}\n"
            "        min: {gain_min}\n"
            "        max: {gain_max}\n",
            "index"_a=std::to_string(m_index),
            "driver_info"_a=to_string(getDriverInfo(m_index)),
            "trigger_mode"_a=getTriggerMode(),
            "trigger_source"_a=getTriggerSources(),
            "trigger_delay"_a=getTriggerDelay(),
            "auto_exposure_time"_a=getAutoExposureTime(),
            "auto_gain"_a=getAutoGain(),
            "et_curr"_a=v_et.current,
            "et_min"_a=v_et.min,
            "et_max"_a=v_et.max,
            "gain_curr"_a=v_g.current,
            "gain_min"_a=v_g.min,
            "gain_max"_a=v_g.max);

//    std::string info = "Param Info: ";
//    info += "index: " + std::to_string(m_index) + "; ";
//    info += to_string(getDriverInfo(m_index)) + "\n";
//    info += "    Trigger Mode: " + getTriggerMode() + "\n";
//    info += "    Trigger Source: " + getTriggerSources() + "\n";
//    info += "    Trigger Delay: " + std::to_string(getTriggerDelay()) + "\n";
//    info += "    Auto Exposure Time: " + getAutoExposureTime() + "\n";
//    info += "    Auto Gain: " + getAutoGain() + "\n";
//    auto v = getExposureTime();
//    info += "    Exposure Time: "
//            "\n        current: " + std::to_string(v.current) +
//            "\n        min: " + std::to_string(v.min) +
//            "\n        max: " + std::to_string(v.max) + "\n";
//    v = getGain();
//    info += "    Gain: "
//            "\n        current: " + std::to_string(v.current) +
//            "\n        min: " + std::to_string(v.min) +
//            "\n        max: " + std::to_string(v.max) + "\n";

    return info;
}

void HikDriver::showParamInfo() const {
    LOG(INFO) << fmt::format("\n=========================================\n"
                             "{}"
                             "=========================================\n",
                             getParamInfo());
}

DriverInfo HikDriver::getDriverInfo(const int& index) const {
    if (index >= m_devices.nDeviceNum) return {};

    MV_CC_DEVICE_INFO* driver_info = m_devices.pDeviceInfo[index];
    if (driver_info == nullptr) return {};

    DriverInfo info;
//    reinterpret_cast<char*>(
//            driver_info->SpecialInfo.stGigEInfo.chUserDefinedName),
//            sizeof(driver_info->SpecialInfo.stGigEInfo.chUserDefinedName);
    if (driver_info->nTLayerType == MV_GIGE_DEVICE) {
        unsigned int nIp1 = ((driver_info->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        unsigned int nIp2 = ((driver_info->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        unsigned int nIp3 = ((driver_info->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        unsigned int nIp4 = (driver_info->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
        info.ip = fmt::format("{}.{}.{}.{}", nIp1, nIp2, nIp3, nIp4);
        info.type = "Gige";
        info.name = reinterpret_cast<char*>(
                driver_info->SpecialInfo.stGigEInfo.chModelName),
                sizeof(driver_info->SpecialInfo.stGigEInfo.chModelName);
    } else if (driver_info->nTLayerType == MV_USB_DEVICE) {
        info.type = "USB";
        info.name = reinterpret_cast<char*>(
                driver_info->SpecialInfo.stUsb3VInfo.chModelName),
                sizeof(driver_info->SpecialInfo.stUsb3VInfo.chModelName);
    } else {
        info.type = "Unknown";
    }
    return info;
}

bool HikDriver::connectDriver() {
    bool is_ok = this->findDriver() && this->openDriver(m_index) && this->initFrameInfo();
    LOG_IF(INFO, is_ok) << "Connect hik => " << to_string(getDriverInfo(m_index));
    return is_ok;
}

bool HikDriver::connectDriver(const int& index) {
    m_index = index;
    return connectDriver();
}

bool HikDriver::connectDriver(const int& index, const TriggerSource& trigger) {
    m_is_trigger = true;
    m_trigger_source = trigger;
    return connectDriver();
}

bool HikDriver::findDriver(int mode) {
    unsigned int error_code;
    error_code = MV_CC_EnumDevices(mode, &m_devices);
    if (!checkErrorCode(error_code, "findDriver") || m_devices.nDeviceNum <= 0) {
        m_opened_status = m_initialized_frame_info = false;
        LOG(WARNING) << "No hik camera devices";
        return false;
    }
    LOG(INFO) << fmt::format("Number of available Hik-devices: {}", m_devices.nDeviceNum);
    return true;
}

bool HikDriver::openDriver(int index) {
    unsigned int error_code;
    error_code = MV_CC_CreateHandle(&m_handle, m_devices.pDeviceInfo[index]);
    if (!checkErrorCode(error_code, "MV_CC_CreateHandle")) return false;
    error_code = MV_CC_OpenDevice(m_handle);
    if (!checkErrorCode(error_code, "MV_CC_OpenDevice")) return false;
    error_code = MV_CC_SetEnumValue(m_handle, "TriggerMode", static_cast<unsigned int>(m_is_trigger));
    if (!checkErrorCode(error_code, "MV_CC_EnumValue => TriggerMode")) return false;
    if (m_is_trigger) setTriggerSource(m_trigger_source);
    error_code = MV_CC_StartGrabbing(m_handle);
    if (!checkErrorCode(error_code, "openDriver Failed!"))
        return false;
    m_opened_status = true;
    return true;
}

bool HikDriver::initFrameInfo() {
    if (!m_opened_status) {
        LOG(ERROR) << "driver no opened!";
        return false;
    }

    unsigned int error_code;
    error_code = MV_CC_GetIntValue(m_handle, "PayloadSize", &m_frame_size_info);
    checkErrorCode(error_code, "GetIntValue => PayloadSize");
    m_data_size = m_frame_size_info.nCurValue;
    m_initialized_frame_info = true;
    return true;
}

HikFrame HikDriver::getFrame() const {
    std::shared_lock<std::shared_mutex> lk(m_frame_mutex);
    return m_hik_frame;
}

void HikDriver::getFrame(HikFrame& frame) {
    std::shared_lock<std::shared_mutex> lk(m_frame_mutex);
    frame = HikFrame(m_hik_frame.m_rgb_frame, m_hik_frame.m_timestamp);
}

void HikDriver::getFrame(HikFrame& frame, const char& method) {
//    if (m_is_trigger) return;

    bool is_ok;
    auto buffer = new unsigned char[m_data_size];
//    auto buffer = std::make_shared<unsigned char>(m_data_size);
//    auto buffer = std::allocate_shared<unsigned char[]>(
//            std::allocator<unsigned char>(), m_data_size);

    if (method == 0) {
        MV_FRAME_OUT_INFO_EX frame_info;
        is_ok = readImageData(buffer, frame_info);
        if (is_ok) {
            cv::Mat f(frame_info.nHeight, frame_info.nWidth, CV_8UC1, buffer);
//            cv::Mat f(frame_info.nHeight, frame_info.nWidth, CV_8UC1, buffer.getView());
            if (!m_is_trigger && &frame == &m_hik_frame) {
                std::lock_guard<std::shared_mutex> lk(m_frame_mutex);
                cv::cvtColor(f, frame.m_rgb_frame, cv::COLOR_BayerRG2RGB);
                frame.m_timestamp = frame_info.nHostTimeStamp;
            } else {
                cv::cvtColor(f, frame.m_rgb_frame, cv::COLOR_BayerRG2RGB);
                frame.m_timestamp = frame_info.nHostTimeStamp;
            }
        } else {
            LOG(WARNING) << "[Frame stream thread]: No image data!";
        }
    } else if (method == 1) {
        MV_FRAME_OUT hik_out;
        is_ok = readImageData(hik_out);
        if (!is_ok) {
            LOG(WARNING) << "[Frame stream thread]: No image data!";
            return;
        }
        cv::Mat f(hik_out.stFrameInfo.nHeight, hik_out.stFrameInfo.nWidth, CV_8UC1, hik_out.pBufAddr);
//            cv::Mat f(frame_info.nHeight, frame_info.nWidth, CV_8UC1, buffer.getView());
        unsigned int error_code;
        {
            std::lock_guard<std::shared_mutex> lk(m_frame_mutex);
            cv::cvtColor(f, frame.m_rgb_frame, cv::COLOR_BayerRG2RGB);
            if (frame.empty()) {
                LOG(ERROR) << "Failed: frame empty!";
            }
            frame.m_timestamp = hik_out.stFrameInfo.nHostTimeStamp;
            error_code = MV_CC_FreeImageBuffer(m_handle, &hik_out);
        }
        checkErrorCode(error_code, "FreeImageBuffer");
    } else {
        LOG(WARNING) << "[Frame stream thread]: No image data!";
    }
    delete[] buffer;
}

void HikDriver::startReadThread() {
    m_read_thread = std::move(std::thread([this]()->void {
        readImageFunction();
    }));
    m_read_thread.detach();
}

bool HikDriver::readImageData(unsigned char* data_buffer,
                              MV_FRAME_OUT_INFO_EX& frame_info,
                              const unsigned int& timeout_ms) {
    if (!m_initialized_frame_info) {
        LOG(ERROR) << "failed: frame info is not initialize!";
        return false;
    }
    unsigned int status;
    status = MV_CC_GetOneFrameTimeout(m_handle, data_buffer, m_data_size, &frame_info, timeout_ms);
    checkErrorCode(status, "readImageData()");
    return status == MV_OK;
}

bool HikDriver::readImageData(MV_FRAME_OUT& frame, const unsigned int& timeout_ms) {
    if (!m_initialized_frame_info) {
        LOG(ERROR) << "failed: frame info is not initialize!";
        return false;
    }
    unsigned int status;
    status = MV_CC_GetImageBuffer(m_handle, &frame, timeout_ms);
    return checkErrorCode(status, "readImageData()");
}

void HikDriver::triggerImageData(HikFrame& frame) {
    if (!m_initialized_frame_info) {
        LOG(ERROR) << "failed: frame info is not initialize!";
        return;
    }
    if (!m_is_trigger) {
        LOG(WARNING) << "非触发模式, 但是调用了触发函数";
        return;
    }

    unsigned int error_code;
    error_code = MV_CC_SetCommandValue(m_handle, "TriggerSoftware");
    checkErrorCode(error_code, "triggerImageData()");
    getFrame(frame, 1);
}

void HikDriver::setTriggerMode(bool mode) {
    unsigned int error_code;
    error_code = MV_CC_SetEnumValue(m_handle, "TriggerMode", static_cast<unsigned int>(mode));
    if (checkErrorCode(error_code, "MV_CC_EnumValue => TriggerMode")) {
        m_is_trigger = mode;
    };
}

std::string HikDriver::getTriggerMode() const {
    MVCC_ENUMVALUE r;
    MV_CC_GetEnumValue(m_handle, "TriggerMode", &r);
    std::string res = r.nCurValue? "ON": "OFF";
    return res;
}

std::string HikDriver::getTriggerSources() const {
    MVCC_ENUMVALUE r;
    MV_CC_GetEnumValue(m_handle, "TriggerSource", &r);
    switch (r.nCurValue) {
        case 0: return "Line0";
        case 1: return "Line1";
        case 2: return "Line2";
        case 3: return "Line3";
        case 4: return "Counter0";
        case 7: return "Software";
        case 8: return "FrequencyConverter";
        default: return "NULL";
    }
}

void HikDriver::setTriggerSource(const TriggerSource& trigger) {
    if (!m_is_trigger) return;

    unsigned int error_code;
    error_code = MV_CC_SetEnumValue(m_handle, "TriggerSource", static_cast<unsigned int>(trigger));
    if (checkErrorCode(error_code, "setTriggerSource")) m_trigger_source = trigger;
}

float HikDriver::getTriggerDelay() const {
    MVCC_FLOATVALUE val;
    unsigned int error_code;
    error_code = MV_CC_GetFloatValue(m_handle, "TriggerDelay", &val);
    float res = checkErrorCode(error_code, "getExposureTime()")? val.fCurValue: 0.f;
    return res;
}

void HikDriver::setTriggerDelay(const float& delay_us) {
    unsigned int error_code;
    error_code = MV_CC_SetFloatValue(m_handle, "TriggerDelay", delay_us);;
    checkErrorCode(error_code, "setTriggerDelay");
}

void HikDriver::setExposureTimeRange(int lower, int upper) {
    unsigned int error_code;
    error_code = MV_CC_SetIntValue(m_handle, "AutoExposureTimeLowerLimit", lower);
    checkErrorCode(error_code, "setExposureTimeRange() - lower");
    error_code = MV_CC_SetIntValue(m_handle, "AutoExposureTimeUpperLimit", upper);
    checkErrorCode(error_code, "setExposureTimeRange() - upper");
}

void HikDriver::setGainRange(int lower, int upper) {
    unsigned int error_code;
    error_code = MV_CC_SetIntValue(m_handle, "AutoGainLowerLimit", lower);
    checkErrorCode(error_code, "setGainRange() - lower");
    error_code =  MV_CC_SetIntValue(m_handle, "AutoGainUpperLimit", upper);
    checkErrorCode(error_code, "setGainRange() - upper");
}

void HikDriver::setAuto(int brightness) {
    /* 自动曝光、自动增益、亮度255 */
    unsigned int error_code;
    error_code = MV_CC_SetEnumValue(m_handle, "ExposureAuto", 2);
    checkErrorCode(error_code, "setAuto() => ExposureAuto");
    error_code = MV_CC_SetEnumValue(m_handle, "GainAuto", 2);
    checkErrorCode(error_code, "setAuto() => GainAuto");
    error_code = MV_CC_SetIntValue(m_handle, "Brightness", brightness);
    checkErrorCode(error_code, "setAuto() => Brightness");
}

void HikDriver::setAutoExposureTime(int mode) {
    unsigned int error_code;
    error_code = MV_CC_SetEnumValue(m_handle, "ExposureAuto", mode);
    checkErrorCode(error_code, "setAutoExposureTime()");
}

void HikDriver::setAutoGain(int mode) {
    unsigned int error_code;
    error_code = MV_CC_SetEnumValue(m_handle, "GainAuto", mode);
    checkErrorCode(error_code, "setAutoGain()");
}

std::string HikDriver::getAutoExposureTime() const {
    MVCC_ENUMVALUE r;
    unsigned int error_code;
    error_code = MV_CC_GetEnumValue(m_handle, "ExposureAuto", &r);
    if (checkErrorCode(error_code, "getAutoExposureTime()")) {
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
    } else {
        return "NULL";
    }
}

std::string HikDriver::getAutoGain() const {
    MVCC_ENUMVALUE r;
    unsigned int error_code;
    error_code =  MV_CC_GetEnumValue(m_handle, "GainAuto", &r);
    if (checkErrorCode(error_code, "getAutoGain()")) {
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
    } else {
        return "NULL";
    }
}

void HikDriver::setExposureTime(float timems) {
    unsigned int error_code;
    error_code = MV_CC_SetEnumValue(m_handle, "ExposureAuto", 0);
    checkErrorCode(error_code, "setExposureTime() => off ExposureAuto");
    error_code =  MV_CC_SetFloatValue(m_handle, "ExposureTime", timems);
    checkErrorCode(error_code, "setExposureTime()");
}

HikDriver::ParamInfo<float> HikDriver::getExposureTime() const {
    MVCC_FLOATVALUE val;
    unsigned int error_code;
    error_code = MV_CC_GetFloatValue(m_handle, "ExposureTime", &val);
    checkErrorCode(error_code, "getExposureTime()");
    return HikDriver::ParamInfo<float>(val);
}

void HikDriver::setGain(float val) {
    unsigned int error_code;
    error_code = MV_CC_SetEnumValue(m_handle, "GainAuto", 0);
    checkErrorCode(error_code, "setGain() => off GainAuto");
    error_code = MV_CC_SetFloatValue(m_handle, "Gain", val);
    checkErrorCode(error_code, "setGain()");
}

HikDriver::ParamInfo<float> HikDriver::getGain() const {
    MVCC_FLOATVALUE val;
    unsigned int error_code = MV_CC_GetFloatValue(m_handle, "Gain", &val);
    checkErrorCode(error_code, "getGain()");
    return HikDriver::ParamInfo<float>(val);
}

void HikDriver::saveAccess(const std::string& user_file, const std::string& device_file) {
    MV_CC_FILE_ACCESS file_access = {};
    file_access.pUserFileName = user_file.c_str();
    file_access.pDevFileName = device_file.c_str();

    unsigned int error_code = MV_CC_FileAccessWrite(m_handle, &file_access);
    checkErrorCode(error_code, "saveAccess()");
}

void HikDriver::loadAccess(const std::string& user_file, const std::string& device_file) {
    MV_CC_FILE_ACCESS file_access = {};
    file_access.pUserFileName = user_file.c_str();
    file_access.pDevFileName = device_file.c_str();

    unsigned int error_code = MV_CC_FileAccessRead(m_handle, &file_access);
    checkErrorCode(error_code, "loadAccess()");
}

void HikDriver::readImageFunction() {
    if (m_data_size <= 0) {
        LOG(ERROR) << fmt::format("Failed: data size {}", m_data_size);
        return;
    }
    LOG_IF(INFO, !m_is_trigger) << "Hik frame stream starting...";

    m_read_thread_running = true;
    while (!m_is_trigger && m_read_thread_running)
        getFrame(m_hik_frame, 1);
    m_read_thread_running = false;
    m_hik_frame = HikFrame();
}

bool HikDriver::checkErrorCode(const unsigned int& code, const std::string& mess) {
    if (code == MV_OK) return true;

    std::string error_mess;
    try {
        error_mess = m_ErrorMess.at(code);
    } catch (const std::out_of_range& a) {
        error_mess = "";
    }
    LOG(ERROR) << fmt::format("{} error code: [0x{:02x}] => {}\n", mess, code, error_mess);
    return false;
}

const std::map<unsigned int, std::string> HikDriver::m_ErrorMess {
        {0x80000000, "错误或无效的句柄"},
        {0x80000001, "不支持的功能"},
        {0x80000004, "错误的参数"},
        {0x80000006, "资源申请失败"},
        {0x80000007, "无数据"},
        {0x800000FF, "未知的错误"},
        {0x80000100, "参数非法"},
        {0x80000107, "超时"},
        {0x800000FF, "未知的错误"},
        {0x00008001, "设备断开连接"},
        {0x8000000D, "没有可输出的缓存"},
        {0x80000106, "节点访问条件有误"}
};

std::string to_string(const DriverInfo& info) {
    std::string res;
    res += "[DriverInfo => ";
    res += fmt::format("name: {}; ", info.name);
    res += fmt::format("type: {}; ", info.type);
    res += fmt::format("ip: {};", info.ip);
    res += "]";
    return res;
}
