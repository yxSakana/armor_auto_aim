/**
 * @project_name armor_auto_aiming
 * @file armor_auto_aim.h
 * @brief
 * @author yx
 * @data 2023-11-14 21:13:13
 */

#ifndef ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H
#define ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H

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

namespace armor_auto_aim {
class ArmorAutoAim {
public:
    ArmorAutoAim();

    void armorAutoAim();
private:
    static constexpr uint8_t  m_SendAutoAimFuncCode = 0;
    static constexpr uint16_t m_SendId = 0;

    ThreadPool m_thread_pool;
    std::unique_ptr<HikDriver> m_hik_driver;
    std::unique_ptr<HikReadThread> m_hik_read_thread;
    std::unique_ptr<HikDebugUi> m_hik_debug_ui;
    Detector m_detector;
    Tracker m_tracker;
    VCOMCOMM m_serial_port;
    PlotClientHttp m_plot_client_http;

    HikFrame m_hik_frame;
    cv::Mat m_frame;
    std::vector<Armor> m_armors;
    float dt = 0.0f;

    void initHikCamera();

    void sendCreateViewRequest();

    void initEkf();
    Eigen::Matrix<double, 6, 6> F;
    Eigen::Matrix<double, 3, 6> H;
    Eigen::DiagonalMatrix<double, 6> Q;
    Eigen::DiagonalMatrix<double, 3> R;
    Eigen::VectorXd X;
    Eigen::VectorXd Z;
};
}

#endif //ARMOR_AUTO_AIMING_ARMOAR_AUTO_AIM_H
