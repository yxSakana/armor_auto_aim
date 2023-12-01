/**
 * @project_name armor_auto_aiming
 * @file armor_auto_aim.h.cpp
 * @brief
 * @author yx
 * @data 2023-11-14 21:13:26
 */

#include <google_logger/google_logger.h>
#include <armor_auto_aim/armor_auto_aim.h>
#include <solver/coordinate_solver.h>
#include <solver/ballistic_solver.h>
#include <debug_toolkit/draw_package.h>

namespace armor_auto_aim {
ArmorAutoAim::ArmorAutoAim()
    : m_hik_driver(std::make_unique<HikDriver>(0)),
      m_hik_read_thread(std::make_unique<HikReadThread>(m_hik_driver.get()))
       {
    initHikCamera();
    initEkf();
#ifdef DEBUG
    sendCreateViewRequest();
#endif
}

void ArmorAutoAim::armorAutoAim() {
#ifdef DEBUG
    cv::namedWindow("frame", cv::WINDOW_NORMAL);
#endif
    QByteArray data;
    cv::TickMeter tick_meter;
    cv::TickMeter send_meter;
    float fps = 0.0f;
    int frame_count = 0;
    constexpr int from_fps_frame_count = 10;
    bool is_reset = true;
    int64_t timestamp = 0;
    int64_t last_t = std::chrono::system_clock::now().time_since_epoch().count();

    while (true) {
        // start fps clock
        if (is_reset) {
            tick_meter.reset();
            tick_meter.start();
            is_reset = false;
        }
        send_meter.reset();
        send_meter.start();
        // -- main --
        // 获取当前时间点
        m_hik_frame = m_hik_read_thread->getFrame();
        m_frame = m_hik_frame.getRgbFrame();
        timestamp = m_hik_frame.getTimestamp();
//        LOG(INFO) << fmt::format("system_timestamp: {}; hik_timestamp: {}",
//                                 std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
//                                 timestamp);
        // -- detect --
        m_detector.detect(m_frame, &m_armors);
        // dt
        dt = static_cast<float>(timestamp - last_t);
        last_t = timestamp;
        // -- tracker --
        if (m_tracker.state() == TrackerStateMachine::State::Lost) {
            m_tracker.initTracker(m_armors);
        } else {
            m_tracker.updateTracker(m_armors);
        }
        // -- send --
        if (m_tracker.state() == TrackerStateMachine::State::Tracking ||
            m_tracker.state() == TrackerStateMachine::State::TempLost) {
            Eigen::Vector3d predict_translation = ballisticCompensate();
            double delta_pitch = solver::ballisticSolver(predict_translation, m_BallisticSpeed);
            CommunicateProtocol communicate_protocol =
                    ArmorAutoAim::translation2YawPitch(predict_translation);
            if (!std::isnan(delta_pitch)) {
                LOG(INFO) << "delta_pitch: " << delta_pitch;
                communicate_protocol.pitch -= static_cast<float>(delta_pitch);
            } else {
                LOG(WARNING) << "delta_pitch is NaN";
            }

            if ((-40.0 < communicate_protocol.yaw && communicate_protocol.yaw < 40.0) &&
                (-40.0 < communicate_protocol.pitch && communicate_protocol.pitch < 40.0)) {
                data.clear();
                data.resize(sizeof(communicate_protocol));
                std::memcpy(data.data(), &communicate_protocol, sizeof(communicate_protocol));
                send_meter.stop();
                emit m_serial_port.CrossThreadTransmitSignal(m_SendAutoAimFuncCode, m_SendId, data);
                LOG(INFO) << "Send to control! " << communicate_protocol.to_string();
                LOG(INFO) << fmt::format("send t: {} Hz", 1 / send_meter.getTimeSec());
                m_thread_pool.enqueue(yaw_pitch_view::yawPitchViewUpdateDataRequest,
                                      &m_plot_client_http,
                                      communicate_protocol);
            } else {
                send_meter.stop();
                LOG(WARNING) << fmt::format("\narmors_size: {};\ntracked_armor-pose: {}; ",
                                            m_armors.size(),
                                            m_tracker.tracked_armor.pose.to_string())
                             << "\npredict: " << predict_translation << "; "
                             << "\ntarget_predict_state: " << m_tracker.getTargetPredictSate()
                             << "\ncommunicate info: " << communicate_protocol.to_string();
                LOG(WARNING) << "异常值!";
            }
        }
        // -- debug --
        // - view -
#ifdef DEBUG
        m_thread_pool.enqueue(armor_auto_aim::ekf_plot::lineSystemUpdateDataRequest, &m_plot_client_http, m_tracker);
        m_thread_pool.enqueue(armor_auto_aim::pnp_view::pnpViewUpdateDataRequest, &m_plot_client_http, m_tracker);
#endif
        // - append information to frame(fps, tracker_info, timestamp, armors) -
        if (frame_count < from_fps_frame_count) {
            frame_count++;
        } else {
            tick_meter.stop();
            fps = static_cast<float>(++frame_count) / static_cast<float>(tick_meter.getTimeSec());
            frame_count = 0;
            is_reset = true;
        }
#ifdef DEBUG
        debug_toolkit::drawFrameInfo(m_frame, m_armors, m_tracker, fps, timestamp, dt);
        cv::imshow("frame", m_frame);
        cv::waitKey(1);
#else
        LOG_EVERY_N(INFO, 10) << fmt::format("fps: {}", fps);
#endif
    }
}

void ArmorAutoAim::initHikCamera() {
    if (m_hik_driver->isConnected()) {
        LOG(INFO) << "Hik Connected!";
        m_hik_driver->setExposureTime(16000);
        m_hik_driver->setGain(10);
        m_hik_driver->showParamInfo();
        m_hik_read_thread->start();
        std::this_thread::sleep_for(std::chrono::seconds(2));

#ifdef DEBUG
        m_hik_debug_ui = std::make_unique<HikDebugUi>(*m_hik_driver);
        m_hik_debug_ui->show();
#endif
    } else {
        LOG(FATAL) << "Hik can't connected!";
    }
}

void ArmorAutoAim::sendCreateViewRequest() {
    ekf_plot::lineSystemCreateWindowRequest(&m_plot_client_http);
    pnp_view::pnpViewCreateWindowRequest(&m_plot_client_http);
    yaw_pitch_view::yawPitchViewCreateWindowRequest(&m_plot_client_http);
}

void ArmorAutoAim::initEkf() {
    //  xa  vxa  ya  vya  za  vza  yaw v_yaw
    Q << q,  0,   0,  0,  0,   0,   0,  0, // xa
         0,  q,   0,  0,  0,   0,   0,  0,// vxa
         0,  0,   q,  0,  0,   0,   0,  0,// ya
         0,  0,   0,  q,  0,   0,   0,  0,// vya
         0,  0,   0,  0,  q,   0,   0,  0,// za
         0,  0,   0,  0,  0,   q,   0,  0,// vza
         0,  0,   0,  0,  0,   0,   q,  0,// yaw
         0,  0,   0,  0,  0,   0,   0,  q;// v_yaw
    //  xa   ya  za  yaw
    R << r,  0,   0,  0, // xa
         0,  r,   0,  0,// vxa
         0,  0,   r,  0,// ya
         0,  0,   0,  r;// vya
//    Q.setIdentity();
//    R.setIdentity();
    Eigen::DiagonalMatrix<double, 8> p0;
    p0.setIdentity();
    //   x  vx  y  vy   z   yz  yaw v_yaw
    F << 1, dt, 0,  0,  0,  0,   0,  0,   // x
         0, 1,  0,  0,  0,  0,   0,  0,   // vx
         0, 0,  1,  dt, 0,  0,   0,  0,   // y
         0, 0,  0,  1,  0,  0,   0,  0,   // vy
         0, 0,  0,  0,  1,  dt,  0,  0,   // z
         0, 0,  0,  0,  0,  1,   0,  0,   // vz
         0, 0,  0,  0,  0,  0,   1,  dt,  // yaw
         0, 0,  0,  0,  0,  0,   0,  1;   // v_yaw
    //    x  vx  y  vy  z  yz yaw  v_yaw
    H <<  1, 0,  0, 0,  0, 0,   0,  0, // x
          0, 0,  1, 0,  0, 0,   0,  0, // y
          0, 0,  0, 0,  1, 0,   0,  0, // z
          0, 0,  0, 0,  0, 0,   1,  0; // yaw
    auto f = [this](const Eigen::MatrixXd& x)->Eigen::MatrixXd {
        return m_tracker.ekf->getF() * x;
    };
    auto h = [this](const Eigen::MatrixXd& x)->Eigen::MatrixXd {
        return m_tracker.ekf->getH() * x;
    };
    auto j_f = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
        Eigen::Matrix<double, 8, 8> F;
        //   x  vx  y  vy   z   yz  yaw v_yaw
        F << 1, dt, 0,  0,  0,  0,   0,  0,   // x
             0, 1,  0,  0,  0,  0,   0,  0,   // vx
             0, 0,  1,  dt, 0,  0,   0,  0,   // y
             0, 0,  0,  1,  0,  0,   0,  0,   // vy
             0, 0,  0,  0,  1,  dt,  0,  0,   // z
             0, 0,  0,  0,  0,  1,   0,  0,   // vz
             0, 0,  0,  0,  0,  0,   1,  dt,  // yaw
             0, 0,  0,  0,  0,  0,   0,  1;   // v_yaw
        return F;
    };
    auto j_h = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
        return H;
    };
    auto update_Q = [this]()->Eigen::MatrixXd {
        if (m_tracker.ekf->getQ().size() == 0) {
            return Q;
        } else {
            return m_tracker.ekf->getQ();
        }
    };
    auto update_R = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
        if (m_tracker.ekf->getR().size() == 0) {
            return R;
        } else {
            return m_tracker.ekf->getR();
        }
    };

    m_tracker.ekf = std::make_shared<ExtendedKalmanFilter>(p0, f, h, j_f, j_h, update_Q, update_R);
}

CommunicateProtocol ArmorAutoAim::translation2YawPitch(const solver::Pose& pose) {
    return {
        static_cast<float>(atan2(pose.x, pose.z) * 180.0f / M_PI),
        static_cast<float>(atan2(pose.y, pose.z) * 180.0f / M_PI),
        pose.z
    };
}

CommunicateProtocol ArmorAutoAim::translation2YawPitch(const Eigen::Vector3d& translation) {
    return {
        static_cast<float>(atan2(translation(0), translation(2)) * 180.0f / M_PI),
        static_cast<float>(atan2(translation(1), translation(2)) * 180.0f / M_PI),
        static_cast<float>(translation(2))
    };
}

Eigen::Vector3d ArmorAutoAim::ballisticCompensate() {
    const Eigen::VectorXd& predict_state = m_tracker.getTargetPredictSate();
    Eigen::Vector3d predict_translation(predict_state(0),
                                        predict_state(2),
                                        predict_state(4));
    auto point = coordinate_solver::reproject(m_intrinsic_matrix, predict_translation);
    cv::circle(m_frame, point, 6, cv::Scalar(59, 188, 235), -1);
    // 弹道模型
//    double delta_pitch = solver::ballisticSolver(predict_translation, m_BallisticSpeed);
//    if (!std::isnan(delta_pitch))
//        predict_translation[1] = predict_translation[2] * sin();
//        predict_translation[1] = tan(atan2(predict_translation[1], predict_translation[2]) - delta_pitch);
//    else
//        LOG(WARNING) << "delta_pitch is NaN!";
//     安装偏移
//    predict_translation[0] += predict_translation[2] * cos(m_OffsetYaw * M_PI / 180);
    predict_translation += Eigen::Vector3d(0.18, -0.27, 0);
    // 延迟
    predict_translation[0] += predict_state[1] * (m_DeltaTime + predict_translation.norm() / m_BallisticSpeed); // FIXME: +-
    // 重投影
    point = coordinate_solver::reproject(m_intrinsic_matrix, predict_translation);
    cv::circle(m_frame, point, 6, cv::Scalar(0, 0, 235), -1);
    return predict_translation;
}
}
