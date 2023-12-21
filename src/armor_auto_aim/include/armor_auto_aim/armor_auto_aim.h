/**
 * @project_name armor_auto_aiming
 * @file armor_auto_aim.h
 * @brief
 * @author yx
 * @data 2023-11-14 21:13:13
 */

#ifndef ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H
#define ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H

#include <mutex>
#include <condition_variable>

#include <yaml-cpp/yaml.h>

#include <threadsafe_queue/threadsafe_queue.h>
#include <solver/solver.h>
#include <thread_pool/thread_pool.h>
#include <HikDriver/HikDriver.h>
#include <HikDriver/HikFrame.hpp>
#include <HikDriver/HikReadThread.h>
#include <HikDriver/HikDebugUi.h>
#include <armor_detector/detector.h>
#include <armor_tracker/tracker.h>
#include <serial_port/communicate_protocol.h>
#include <serial_port/VCOMCOMM.h>
#include <plot_client_http/ekf_plot.h>
#include <plot_client_http/pnp_view.h>
#include <plot_client_http/yaw_pitch_view.h>
#include <view/view.h>

namespace armor_auto_aim {
struct AutoAimParams {
    float exp_time;
    float gain;
    ArmorColor target_color;
    std::string armor_model_path;
    float delta_time;
};

class ArmorAutoAim {
//    Q_OBJECT
public:
    explicit ArmorAutoAim(const std::string& config_path);

//    void operator()();  TODO: 修改为 () 重载
    void armorAutoAim();
private:
    static constexpr uint16_t m_PcId = 0; // PC 发送消息时的ID
    static constexpr uint8_t m_LastFuncCode = 0; // 未识别到发送的code
    static constexpr uint8_t  m_SendAutoAimFuncCode = 1; // 识别到时发送的自瞄信息
    static constexpr double m_DeltaTime = 50.0;
    int q = 1, r = 1;

    std::string m_config_path;
    YAML::Node m_config;
    AutoAimParams m_params;

    ThreadPool m_thread_pool;
    std::unique_ptr<HikDriver> m_hik_driver;
    std::unique_ptr<HikReadThread> m_hik_read_thread;
#ifdef DEBUG
    std::unique_ptr<HikDebugUi> m_hik_debug_ui;
    view::View m_view;
#endif
    SolverBuilder m_solver_builder;
    Solver m_solver;
    Detector m_detector;
    Tracker m_tracker;
    VCOMCOMM m_serial_port;
    PlotClientHttp m_plot_client_http;
    HikFrame m_hik_frame;
    cv::Mat m_frame;
    std::vector<Armor> m_armors;
    float dt = 0.0f;
    std::shared_ptr<ImuData> m_imu_data = std::make_shared<ImuData>();
    ThreadSafeQueue<ImuData> m_imu_data_queue;

    void loadConfig();

    void initHikCamera();

    void sendCreateViewRequest();

    void initEkf();
    Eigen::Matrix<double, 8, 8> F;
    Eigen::Matrix<double, 4, 8> H;
    Eigen::Matrix<double, 8, 8> Q;
    Eigen::Matrix<double, 4, 4> R;
    Eigen::VectorXd X;
    Eigen::VectorXd Z;

    /**
     * @brief 安装偏移、弹道模型、延迟、到IMU的坐标转换
     */
    Eigen::Vector3d ballisticCompensate();

    inline static PredictData translation2YawPitch(const solver::Pose& pose);

    inline static PredictData translation2YawPitch(const Eigen::Vector3d& translation);
};
}

#endif //ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H
