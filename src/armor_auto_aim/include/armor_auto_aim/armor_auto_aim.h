/**
 * @project_name armor_auto_aiming
 * @file armor_auto_aim.h
 * @brief
 * @author yx
 * @date 2023-11-14 21:13:13
 */

#ifndef ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H
#define ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H

#include <chrono>
#include <mutex>
#include <condition_variable>

#include <yaml-cpp/yaml.h>
#include <QObject>
#include <QThread>
#include <QTimer>

#include <safe_container/threadsafe_queue.h>
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

class ArmorAutoAim: public QThread {
    Q_OBJECT
    using ProgramClock = std::chrono::system_clock;
    using ClockUnit = std::chrono::milliseconds;
public:
    explicit ArmorAutoAim(const std::string& config_path, QObject* parent = nullptr);

    void run() override;
private:
    static constexpr uint16_t m_PcId = 0; // id: PC
    static constexpr uint8_t m_SendAutoAimInfoCode = 1; // code: 发送自瞄信息
    static constexpr uint8_t m_SendTimestampCode = 2; // code: 发送pc当前时间戳
    static constexpr uint16_t m_MicrocontrollerId = 1;  // id: 单片机
    static constexpr uint8_t m_ImuInfoCode = 0;  // code: IMU数据
    static constexpr double m_DeltaTime = 50.0;
    int q = 1, r = 1, p = 10000;

    std::string m_config_path;
    YAML::Node m_config;
    AutoAimParams m_params;

//    ThreadPool m_thread_pool;
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
    QTimer* m_auto_connect_serial;
    HikFrame m_hik_frame;
    cv::Mat m_frame;
    std::vector<Armor> m_armors;
    float dt = 0.0f;
    std::shared_ptr<ImuData> m_imu_data = std::make_shared<ImuData>();
    ThreadSafeQueue<ImuData> m_imu_data_queue;  // FIXME: 不使用队列、会明显滞后、因为每次都是取得是之前的； 另外KF加入速度观测值
    AutoAimInfo m_aim_info;
    QByteArray m_data;

    void loadConfig();

    void initHikCamera();

    void initEkf();
    Eigen::Matrix<double, 8, 8> F;
    Eigen::Matrix<double, 4, 8> H;
    Eigen::Matrix<double, 8, 8> Q;
    Eigen::Matrix<double, 4, 4> R;
    Eigen::VectorXd X;
    Eigen::VectorXd Z;

    inline static AutoAimInfo translation2YawPitch(const solver::Pose& pose);

    inline static AutoAimInfo translation2YawPitch(const Eigen::Vector3d& translation);
signals:
#ifdef DEBUG
    void viewSign(const Eigen::Vector3d& world_predict_translation,
                  const Eigen::Vector3d& predict_translation,
                  const uint64_t& timestamp,
                  const uint64_t& imu_timestamp);
#endif
private slots:
    void funcSelect(uint8_t fun_code, uint16_t id, const QByteArray& data);

    void sendNowTimestamp();

#ifdef DEBUG
    void view(const Eigen::Vector3d& world_predict_translation,
              const Eigen::Vector3d& predict_translation,
              const uint64_t& timestamp,
              const uint64_t& imu_timestamp);
#endif
};
}

#endif //ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H
