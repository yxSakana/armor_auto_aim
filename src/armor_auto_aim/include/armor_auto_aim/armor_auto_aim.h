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

#include <threadsafe_queue/threadsafe_queue.h>
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

namespace armor_auto_aim {
class ArmorAutoAim {
//    Q_OBJECT
public:
    ArmorAutoAim();

    void armorAutoAim();
private:
    static constexpr uint8_t  m_SendAutoAimFuncCode = 0;
    static constexpr uint8_t m_LastFuncCode = 1;
    static constexpr uint16_t m_SendId = 0;
    static constexpr double m_BallisticSpeed = 6.0;
    static constexpr double m_DeltaTime = 200.0;
    static constexpr double m_OffsetYaw = 5.5;
    static constexpr int q = 1, r = 10000;

    const std::array<double, 9> m_intrinsic_matrix {
            2665.005527408399, 0,                  696.8233, // fx 0  cx
            0,                 2673.364537791387,  500.5147099572225, // 0  fy cy
            0,                 0,                  1
    };
    const std::vector<double> m_distortion_vector {-0.303608974614145, 4.163247825941419, -0.008432853056627, -0.003830248744148, 0};
    Eigen::Matrix4d m_T_ci;
    Eigen::Vector3d m_tvec_iw;

    ThreadPool m_thread_pool;
    std::unique_ptr<HikDriver> m_hik_driver;
    std::unique_ptr<HikReadThread> m_hik_read_thread;
#ifdef DEBUG
    std::unique_ptr<HikDebugUi> m_hik_debug_ui;
#endif
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
