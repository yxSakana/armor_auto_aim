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
#include <HikDriver/HikUi.h>
#include <armor_detector/detector.h>
#include <armor_tracker/tracker.h>
#include <serial_port/communicate_protocol.h>
#include <serial_port/VCOMCOMM.h>
//#include <safe_container/safe_stack.h>
#include <safe_container/safe_circular_buffer.h>
#ifdef DEBUG
#include <view/view.h>
#include <armor_auto_aim/view_work.h>
#include <armor_auto_aim/serail_work.h>
#endif

namespace armor_auto_aim {
struct AutoAimParams {
    float exp_time;
    float gain;
    ArmorColor target_color;
    std::string armor_model_path;
    float delta_time;
    float compensate_yaw;
    float compensate_pitch;
};

class ArmorAutoAim: public QThread {
    Q_OBJECT
    using ProgramClock = std::chrono::system_clock;
    using ClockUnit = std::chrono::milliseconds;
public:
    explicit ArmorAutoAim(const std::string& config_path, QObject* parent = nullptr);

    void run() override;

    void setSerialWork(SerialWork* sw);

    void setViewWork(ViewWork* vw);
public slots:
    void pushImuData(const ImuData& data) { m_imu_data_queue.push(data); }
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
    std::string m_config_path;
    YAML::Node m_config;
    AutoAimParams m_params;

    std::unique_ptr<HikDriver> m_hik_driver;
#ifdef DEBUG
    std::unique_ptr<HikUi> m_hik_ui;
    ViewWork* m_view_work;
    SerialWork* m_serial_work;
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
    SafeCircularBuffer<ImuData, 10> m_imu_data_queue;
//    ThreadSafeQueue<ImuData> m_imu_data_queue;  // FIXME: 不使用队列、会明显滞后、因为每次都是取得是之前的； 另外KF加入速度观测值
    AutoAimInfo m_aim_info;

    void loadConfig();

    void initHikCamera();

    void initEkf();
    Eigen::Matrix<double, 8, 8> F;
    Eigen::Matrix<double, 4, 8> H;
//    Eigen::Matrix<double, 8, 8> Q;
//    Eigen::Matrix<double, 4, 4> R;
    Eigen::DiagonalMatrix<double, 8> Q;
    Eigen::DiagonalMatrix<double, 4> R;
    Eigen::VectorXd X;
    Eigen::VectorXd Z;
    Eigen::Vector4d m_r_diagonal;
    Eigen::Matrix<double, 8, 1> m_q_diagonal;

    inline static AutoAimInfo translation2YawPitch(const solver::Pose& pose);

    inline static AutoAimInfo translation2YawPitch(const Eigen::Vector3d& translation);

    static int64_t diffFunction(const HikFrame& camera, const ImuData& imu) {
        return std::abs(static_cast<int64_t>(imu.timestamp) - camera.getTimestamp());
    }
};
}

#endif //ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H
