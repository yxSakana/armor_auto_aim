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
//#include <safe_container/safe_stack.h>
#include <safe_container/safe_circular_buffer.h>

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
public slots:
    void pushImuData(const ImuData& data) { m_imu_data_queue.push(data); }

    void pushCameraData(const HikFrame& frame) { m_camera_stack.push(frame); }
signals:
    void sendAimInfo(const AutoAimInfo& aim_info);

    void showFrame(const cv::Mat& frame,
                   const std::vector<armor_auto_aim::Armor>& armors, const armor_auto_aim::Tracker& tracker,
                   const double& fps, const uint64_t& timestamp, const float& dt);

    void viewEkfSign(const armor_auto_aim::Tracker& tracker,
                     const Eigen::Vector3d& predict_camera_coordinate,
                     const Eigen::Vector3d& shoot_camera_coordinate);

    void viewTimestampSign(const uint64_t& camera_timestamp,
                           const uint64_t& imu_timestamp);
private:
    static constexpr double m_DeltaTime = 50.0;
    int q = 1, r = 1, p = 10000;

    std::string m_config_path;
    YAML::Node m_config;
    AutoAimParams m_params;

    std::unique_ptr<HikDriver> m_hik_driver;
    std::unique_ptr<HikReadThread> m_hik_read_thread;
#ifdef DEBUG
    std::unique_ptr<HikDebugUi> m_hik_debug_ui;
#endif
    SolverBuilder m_solver_builder;
    Solver m_solver;
    Detector m_detector;
    Tracker m_tracker;
    HikFrame m_hik_frame;
    cv::Mat m_frame;
    std::vector<Armor> m_armors;
    float dt = 0.0f;
    std::shared_ptr<ImuData> m_imu_data = std::make_shared<ImuData>();
    SafeCircularBuffer<ImuData, 10> m_imu_data_queue;  // FIXME: 不使用队列、会明显滞后、因为每次都是取得是之前的； 另外KF加入速度观测值
    SafeCircularBuffer<HikFrame, 10> m_camera_stack;
    AutoAimInfo m_aim_info;

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

    static int64_t diffFunction(const HikFrame& camera, const ImuData& imu) {
        return std::abs(static_cast<int64_t>(imu.timestamp) - camera.getTimestamp());
    }
};
}

#endif //ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H
