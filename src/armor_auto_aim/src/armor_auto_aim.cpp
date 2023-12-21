/**
 * @project_name armor_auto_aiming
 * @file armor_auto_aim.h.cpp
 * @brief
 * @author yx
 * @data 2023-11-14 21:13:26
 */

#ifdef DEBUG
#include <QPointF>
#endif

#include <google_logger/google_logger.h>
#include <armor_auto_aim/armor_auto_aim.h>
#include <solver/coordinate_solver.h>
#include <solver/ballistic_solver.h>
#include <debug_toolkit/draw_package.h>

namespace armor_auto_aim {
ArmorAutoAim::ArmorAutoAim(const std::string& config_path)
    : m_hik_driver(std::make_unique<HikDriver>(0)),
      m_hik_read_thread(std::make_unique<HikReadThread>(m_hik_driver.get())),
      m_config_path(config_path)
       {
    loadConfig();
    m_solver = Solver(m_solver_builder.build());
    m_detector = Detector(m_params.armor_model_path, m_solver.pnp_solver);
    initHikCamera();
    initEkf();
#ifdef DEBUG
    sendCreateViewRequest();
#endif
    m_auto_connect_serial = new QTimer(this);
    m_auto_connect_serial->start(1000);
    connect(m_auto_connect_serial, &QTimer::timeout, [this](){
        if ((!this->m_serial_port.isOpen()) || m_serial_port.error() != QSerialPort::NoError) {
            LOG(WARNING) << "Serial close. try auto connect...";
            this->m_serial_port.auto_connect();
        } else {
            LOG(INFO) << "Serial is opened";
        }
    });
    connect(&m_serial_port, &VCOMCOMM::receiveData,
                     [this](uint8_t fun_code, uint16_t id, const QByteArray& data) {
        ImuData imu_data;
        std::memcpy(&imu_data, data, sizeof(imu_data));
        m_imu_data_queue.push(imu_data);
    });
#ifdef DEBUG
    m_view.m_ekf_view->showMaximized();
//    m_view.m_ekf_view->show();
    for (const auto& k: {"x", "v_x", "y", "v_y"})
        m_view.m_ekf_view->get(k)->setShowNumber(300);
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
        bool is_have = m_imu_data_queue.tryPop(*m_imu_data);
        if (!is_have) {
//            m_hik_frame = m_hik_read_thread->getFrame();
//            m_frame = m_hik_frame.getRgbFrame();
//            cv::imshow("frame", m_frame);
//            cv::waitKey(1);
            continue;
        }
//        m_imu_data = m_imu_data_queue.waitPop();
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
        // -- detect --
        m_detector.detect(m_frame, &m_armors);
        // get world coordinate
        Eigen::Vector3d c_point;
        Eigen::Quaterniond quaternion;
        for (auto& armor: m_armors) {
            c_point = Eigen::Vector3d(armor.pose.x, armor.pose.y, armor.pose.z);
            quaternion = Eigen::Quaterniond(m_imu_data->quaternion.w, m_imu_data->quaternion.x,
                                            m_imu_data->quaternion.y, m_imu_data->quaternion.z);
            armor.world_coordinate = m_solver.cameraToWorld(c_point, quaternion.matrix());
        }
        // dt
        dt = static_cast<float>(timestamp - last_t);
        last_t = timestamp;
        // -- tracker --
        if (m_tracker.state() == TrackerStateMachine::State::Lost) {
            m_tracker.initTracker(m_armors);
        } else {
            m_tracker.updateTracker(m_armors);
        }
        // -- predict && send --
        if (m_tracker.state() == TrackerStateMachine::State::Tracking ||
            m_tracker.state() == TrackerStateMachine::State::TempLost) {
            // -- predict --
            const Eigen::VectorXd& predict_state = m_tracker.getTargetPredictSate();
            Eigen::Vector3d predict_translation(predict_state(0), predict_state(2), predict_state(4));
            Eigen::Vector3d world_predict_translation = predict_translation;
            predict_translation = m_solver.worldToCamera(predict_translation, quaternion.matrix());
            auto point = m_solver.reproject(predict_translation);
            cv::circle(m_frame, point, 6, cv::Scalar(59, 188, 235), -1);
            predict_translation[0] -= predict_state[1] * (m_params.delta_time + std::abs(predict_translation.norm() / m_solver.getSpeed()));
            point = m_solver.reproject(predict_translation);
            cv::circle(m_frame, point, 6, cv::Scalar(0, 0, 235), -1);
#ifdef DEBUG // View
            QPointF p;
            // x-measure-word
            p = m_view.m_ekf_view->getLastPoint("x", "measure_world");
            m_view.m_ekf_view->insert("x", "measure_world", p.x()+1, m_tracker.tracked_armor.world_coordinate[0]);
            // x-predict-world
            p = m_view.m_ekf_view->getLastPoint("x", "predict_world");
            m_view.m_ekf_view->insert("x", "predict_world", p.x()+1, world_predict_translation[0]);
            // x-measure
            p = m_view.m_ekf_view->getLastPoint("x", "measure");
            m_view.m_ekf_view->insert("x", "measure", p.x()+1, m_tracker.tracked_armor.pose.x);
            // x-predict
            p = m_view.m_ekf_view->getLastPoint("x", "predict");
            m_view.m_ekf_view->insert("x", "predict", p.x()+1, predict_translation[0]);
            // x-predict_shoot
            p = m_view.m_ekf_view->getLastPoint("x", "predict_shoot");
            m_view.m_ekf_view->insert("x", "predict_shoot", p.x()+1, predict_translation[0]);
            // v_x-predict
            p = m_view.m_ekf_view->getLastPoint("v_x", "predict");
            m_view.m_ekf_view->insert("v_x", "predict", p.x()+1, predict_state[1]);
            // y-measure-word
            p = m_view.m_ekf_view->getLastPoint("y", "measure_world");
            m_view.m_ekf_view->insert("y", "measure_world", p.x()+1, m_tracker.tracked_armor.world_coordinate[1]);
            // y-predict-world
            p = m_view.m_ekf_view->getLastPoint("y", "predict_world");
            m_view.m_ekf_view->insert("y", "predict_world", p.x()+1, world_predict_translation[1]);
            // y-measure
            p = m_view.m_ekf_view->getLastPoint("y", "measure");
            m_view.m_ekf_view->insert("y", "measure", p.x()+1, m_tracker.tracked_armor.pose.y);
            // y-predict
            p = m_view.m_ekf_view->getLastPoint("y", "predict");
            m_view.m_ekf_view->insert("y", "predict", p.x()+1, predict_translation[1]);
            // v_y-predict
            p = m_view.m_ekf_view->getLastPoint("v_y", "predict");
            m_view.m_ekf_view->insert("v_y", "predict", p.x()+1, predict_state[3]);
#endif
            double delta_pitch = m_solver.ballisticSolver(predict_translation);
            PredictData predict_data = translation2YawPitch(predict_translation);
            if (!std::isnan(delta_pitch)) {
                LOG(INFO) << "delta_pitch: " << delta_pitch;
                predict_data.pitch -= static_cast<float>(delta_pitch);
            } else {
                LOG(WARNING) << "delta_pitch is NaN";
            }

            if ((-40.0 < predict_data.yaw && predict_data.yaw < 40.0) &&
                (-40.0 < predict_data.pitch && predict_data.pitch < 40.0)) {
                data.clear();
                data.resize(sizeof(predict_data));
                std::memcpy(data.data(), &predict_data, sizeof(predict_data));
                send_meter.stop();
                emit m_serial_port.CrossThreadTransmitSignal(m_SendAutoAimFuncCode, m_PcId, data);
                LOG(INFO) << "Send to control! " << predict_data.to_string();
                LOG(INFO) << fmt::format("send t: {} Hz", 1 / send_meter.getTimeSec());
                m_thread_pool.enqueue(yaw_pitch_view::yawPitchViewUpdateDataRequest,
                                      &m_plot_client_http,
                                      predict_data);
            } else {
                send_meter.stop();
                emit m_serial_port.CrossThreadTransmitSignal(m_LastFuncCode, m_PcId, "");
                LOG(WARNING) << fmt::format("\narmors_size: {};\ntracked_armor-pose: {}; ",
                                            m_armors.size(),
                                            m_tracker.tracked_armor.pose.to_string())
                             << "\npredict: " << predict_translation << "; "
                             << "\ntarget_predict_state: " << m_tracker.getTargetPredictSate()
                             << "\ncommunicate info: " << predict_data.to_string();
                LOG(WARNING) << "异常值!";
            }
        } else {
            emit m_serial_port.CrossThreadTransmitSignal(m_LastFuncCode, m_PcId, "");
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

void ArmorAutoAim::loadConfig() {
    // detector
    m_config = YAML::LoadFile(m_config_path);
    if (!m_config)
        LOG(FATAL) << fmt::format("Failed: Invalid configuration file: {}!", m_config_path);
    const YAML::Node&& detector_config = m_config["detector"];
    m_params.exp_time = detector_config["camera"]["exposure_time"].as<float>();
    m_params.gain = detector_config["camera"]["gain"].as<float>();
    m_params.armor_model_path = detector_config["armor_model_path"].as<std::string>();
    auto c = detector_config["target_color"].as<std::string>();
    std::transform(c.begin(), c.end(), c.begin(), ::toupper);
    if (c == "RED")        m_params.target_color = ArmorColor::RED;
    else if (c == "BLUE")  m_params.target_color = ArmorColor::BLUE;
    else                   LOG(FATAL) << "Unknown color: " << c << "(transform to all upper case)";
    // solver config
    const YAML::Node&& solver_config = m_config["solver"];
    m_solver_builder.setG(solver_config["env"]["g"].as<double>());
    m_solver_builder.setBallSpeed(solver_config["ballistic"]["speed"].as<double>());
    m_solver_builder.setIntrinsicMatrix(solver_config["camera_params"]["intrinsic_matrix"].as<std::array<double, 9>>());
    m_solver_builder.setDistortionCess(solver_config["camera_params"]["distortion"].as<std::vector<double>>());
    auto T_ic_ci = solver_config["coordinate"]["T_ic"].as<std::vector<double>>();
    m_solver_builder.setTic(Eigen::Map<Eigen::Matrix4d>(T_ic_ci.data(), 4, 4).transpose());
    T_ic_ci = solver_config["coordinate"]["T_ci"].as<std::vector<double>>();
    m_solver_builder.setTci(Eigen::Map<Eigen::Matrix4d>(T_ic_ci.data(), 4, 4).transpose());
    auto tvec_i2w = solver_config["coordinate"]["tvec_i2w"].as<std::array<double, 3>>();
    Eigen::Vector3d tvec = Eigen::Map<Eigen::Vector3d>(tvec_i2w.data(), 3, 1);
    m_solver_builder.setTvecI2C(tvec);
    m_solver_builder.setTvecC2I(-tvec);
    // predict
    const YAML::Node&& predict_config = m_config["predict"];
    m_params.delta_time = predict_config["delta_time"].as<float>();
    q = predict_config["kf"]["q"].as<int>();
    r = predict_config["kf"]["r"].as<int>();
}

void ArmorAutoAim::initHikCamera() {
    if (m_hik_driver->isConnected()) {
        LOG(INFO) << "Hik Connected!";
        m_hik_driver->setExposureTime(m_params.exp_time);
        m_hik_driver->setGain(m_params.gain);
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
         0,  r,   0,  0, // vxa
         0,  0,   r,  0, // ya
         0,  0,   0,  r; // vya
//    Q.setIdentity();
//    R.setIdentity();
    Eigen::DiagonalMatrix<double, 8> p0;
    p0.setIdentity();
    //   x  vx  y  vy   z   yz  yaw v_yaw
    F << 1, dt, 0,  0,  0,  0,   0,  0,  // x
         0, 1,  0,  0,  0,  0,   0,  0,  // vx
         0, 0,  1,  dt, 0,  0,   0,  0,  // y
         0, 0,  0,  1,  0,  0,   0,  0,  // vy
         0, 0,  0,  0,  1,  dt,  0,  0,  // z
         0, 0,  0,  0,  0,  1,   0,  0,  // vz
         0, 0,  0,  0,  0,  0,   1,  dt, // yaw
         0, 0,  0,  0,  0,  0,   0,  1;  // v_yaw
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

PredictData ArmorAutoAim::translation2YawPitch(const solver::Pose& pose) {
    return {
        static_cast<float>(atan2(pose.x, pose.z) * 180.0f / M_PI),
        static_cast<float>(atan2(pose.y, pose.z) * 180.0f / M_PI),
        pose.z
    };
}

PredictData ArmorAutoAim::translation2YawPitch(const Eigen::Vector3d& translation) {
    return {
        static_cast<float>(atan2(translation(0), translation(2)) * 180.0f / M_PI),
        static_cast<float>(atan2(translation(1), translation(2)) * 180.0f / M_PI),
        static_cast<float>(translation(2))
    };
}

Eigen::Vector3d ArmorAutoAim::ballisticCompensate() {
    // FIXME: 修改到 Solver
    const Eigen::VectorXd& predict_state = m_tracker.getTargetPredictSate();
    Eigen::Vector3d predict_translation(predict_state(0),
                                        predict_state(2),
                                        predict_state(4));
    auto point = m_solver.reproject(predict_translation);
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
    predict_translation[0] += predict_state[1] * (m_DeltaTime + predict_translation.norm() / m_solver.getSpeed()); // FIXME: +-
    // 重投影
    point = m_solver.reproject(predict_translation);
    cv::circle(m_frame, point, 6, cv::Scalar(0, 0, 235), -1);
    return predict_translation;
}
}
