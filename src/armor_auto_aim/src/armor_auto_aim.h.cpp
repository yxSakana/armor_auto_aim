/**
 * @project_name armor_auto_aiming
 * @file armor_auto_aim.h.cpp
 * @brief
 * @author yx
 * @data 2023-11-14 21:13:26
 */

#include <google_logger/google_logger.h>
#include <armor_auto_aim/armor_auto_aim.h>
#include <debug_toolkit/draw_package.h>

namespace armor_auto_aim {
ArmorAutoAim::ArmorAutoAim()
    : m_hik_driver(std::make_unique<HikDriver>(0)),
      m_hik_read_thread(std::make_unique<HikReadThread>(m_hik_driver.get()))
       {
    initHikCamera();
    initEkf();
    sendCreateViewRequest();
}

void ArmorAutoAim::armorAutoAim() {
    cv::namedWindow("frame", cv::WINDOW_NORMAL);
    cv::TickMeter tick_meter;
    float fps = 0.0f;
    int64_t timestamp = 0;
    int64_t last_t = std::chrono::system_clock::now().time_since_epoch().count();
    // show debug img
    auto showDebugImg = [this, &tick_meter, &fps, &timestamp]()->void {
        tick_meter.stop();
        fps = 1.0f / static_cast<float>(tick_meter.getTimeSec());
        debug_toolkit::drawFrameInfo(m_frame, m_armors, m_tracker, fps, timestamp, dt);
        cv::imshow("frame", m_frame);
        cv::waitKey(1);
    };

    while (true) {
        // start fps clock
        tick_meter.reset();
        tick_meter.start();
        // -- main --
        m_hik_frame = m_hik_read_thread->getFrame();
        m_frame = m_hik_frame.getRgbFrame();
        timestamp = m_hik_frame.getTimestamp();
        // detect
        m_detector.detect(m_frame, &m_armors);
        // dt
        dt = static_cast<float>(timestamp - last_t);
        last_t = timestamp;
        // tracker
        if (m_tracker.state() == TrackerStateMachine::State::Lost) {
            m_tracker.initTracker(m_armors);
        } else {
            m_tracker.updateTracker(m_armors);
        }
        // send
        if (m_tracker.state() == TrackerStateMachine::State::Tracking ||
            m_tracker.state() == TrackerStateMachine::State::TempLost) {
            CommunicateProtocol communicate_protocol(
                    m_tracker.tracked_armor.pose.yaw,
                    m_tracker.tracked_armor.pose.pitch,
                    m_tracker.tracked_armor.pose.z
                    );
            QByteArray data;
            data.resize(sizeof(communicate_protocol));
            std::memcpy(data.data(), &communicate_protocol, sizeof(communicate_protocol));
            emit m_serial_port.CrossThreadTransmitSignal(m_SendAutoAimFuncCode, m_SendId, data);
        }
        // -- debug --
        // view
        m_thread_pool.enqueue(armor_auto_aim::ekf_plot::lineSystemUpdateDataRequest, &m_plot_client_http, m_tracker);
        m_thread_pool.enqueue(armor_auto_aim::pnp_view::pnpViewUpdateDataRequest, &m_plot_client_http, m_tracker);
        // append information to frame
        showDebugImg();
    }
}

void ArmorAutoAim::initHikCamera() {
    if (m_hik_driver->isConnected()) {
        LOG(INFO) << "Hik Connected!";
        m_hik_driver->setExposureTime(40000);
        m_hik_driver->setGain(10);
        m_hik_driver->showParamInfo();
        m_hik_read_thread->start();
        std::this_thread::sleep_for(std::chrono::seconds(2));

        m_hik_debug_ui = std::make_unique<HikDebugUi>(*m_hik_driver);
        m_hik_debug_ui->show();
    } else {
        LOG(FATAL) << "Hik can't connected!";
    }
}

void ArmorAutoAim::sendCreateViewRequest() {
    ekf_plot::lineSystemCreateWindowRequest(&m_plot_client_http);
    pnp_view::pnpViewCreateWindowRequest(&m_plot_client_http);
}

void ArmorAutoAim::initEkf() {
    //  xa  vxa ya vya za vza
    Q.setIdentity();
    R.setIdentity();
    Eigen::DiagonalMatrix<double, 6> p0;
    p0.setIdentity();
    //   x  vx  y  vy  z  yz
    F << 1, dt, 0,  0,  0, 0,  // x
         0, 1,  0,  0,  0, 0,  // vx
         0, 0,  1,  dt, 0, 0,  // y
         0, 0,  0,  1,  0, 0,  // vy
         0, 0,  0,  0,  1, dt, // z
         0, 0,  0,  0,  1, 1; // vz
    //     x  vx  y  vy  z  yz
    H <<   1, 0,  0, 0,  0, 0, // x
           0, 0,  1, 0,  0, 0, // y
           0, 0,  0, 0,  1, 0; // z
    auto f = [this](const Eigen::MatrixXd& x)->Eigen::MatrixXd {
        return m_tracker.ekf->getF() * x;
    };
    auto h = [this](const Eigen::MatrixXd& x)->Eigen::MatrixXd {
        return m_tracker.ekf->getH() * x;
    };
    auto j_f = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
        Eigen::Matrix<double, 6, 6> F;
        //   x  vx  y  vy   z   yz
        F << 1, dt, 0,  0,  0,  0,  // x
             0, 1,  0,  0,  0,  0,  // vx
             0, 0,  1,  dt, 0,  0,  // y
             0, 0,  0,  1,  0,  0,  // vy
             0, 0,  0,  0,  1,  dt, // z
             0, 0,  0,  0,  0,  1;  // vz
        return F;
    };
    auto j_h = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
        return H;
    };
    auto update_Q = [this]()->Eigen::MatrixXd {
        return Q;
    };
    auto update_R = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
        return R;
    };

    m_tracker.ekf = new ExtendedKalmanFilter(p0, f, h, j_f, j_h, update_Q, update_R);
}
}
