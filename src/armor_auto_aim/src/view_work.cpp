/**
 * @project_name auto_aim
 * @file view.cpp
 * @brief
 * @author yx
 * @date 2023-12-24 18:04:45
 */

#include <glog/logging.h>

#include <QThread>

#include <armor_auto_aim/view_work.h>
#include <debug_toolkit/draw_package.h>

namespace armor_auto_aim {
ViewWork::ViewWork(QObject* parent)
        : QObject(parent),
          m_view(new view::View) {
    qRegisterMetaType<Eigen::Vector3d>("Eigen::Vector3d");
    qRegisterMetaType<uint64_t>("uint64_t");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::vector<armor_auto_aim::Armor>>("std::vector<armor_auto_aim::Armor>");
    qRegisterMetaType<armor_auto_aim::Tracker>("armor_auto_aim::Tracker");

    cv::namedWindow("frame", cv::WINDOW_NORMAL);

    connect(this, &ViewWork::viewFaceAngleSign, this, &ViewWork::viewFaceAngle);

    for (const auto& k: {"x", "v_x", "y", "v_y"})
        m_view->m_ekf_view->getView(k)->setShowNumber(100);
    m_view->m_timestamp_view->getView("Camera-IMU timestamp")->setShowNumber(100);
    m_view->m_imu_euler->getView("Euler")->setShowNumber(100);
}

void ViewWork::show() {
    LOG(INFO) << "view imshow(): " << QThread::currentThreadId();
    m_view->m_ekf_view->showMaximized();
//    m_view->m_timestamp_view->showMaximized();
//    m_view->m_imu_euler->showMaximized();
    m_view->m_face_angle_view->showMaximized();
}

void ViewWork::showFrame(const cv::Mat& frame,
                         const std::vector<Armor>& armors, const armor_auto_aim::Tracker& tracker,
                         const double& fps, const uint64_t& timestamp, const float& dt) {
    cv::Mat f = frame.clone();
    debug_toolkit::drawFrameInfo(f, armors, tracker, fps, timestamp, dt);
    cv::imshow("frame", f);
    cv::waitKey(1);
}

void ViewWork::viewEkf(
        const armor_auto_aim::Tracker& tracker,
        const Eigen::Vector3d& predict_camera_coordinate,
        const Eigen::Vector3d& shoot_camera_coordinate) const {
    QPointF p;
    const Eigen::VectorXd& predict_state = tracker.getTargetPredictSate();
    const Eigen::Vector3d predict_world_coordinate(
            predict_state[0], predict_state[2], predict_state[4]);
    Armor armor =  tracker.tracked_armor;
    // --- EKF-View ---
    // x-measure-word
    p = m_view->m_ekf_view->getLastPoint("x", "measure_world");
    m_view->m_ekf_view->insert("x", "measure_world", p.x()+1, armor.world_coordinate[0]);
    // x-predict-world
    p = m_view->m_ekf_view->getLastPoint("x", "predict_world");
    m_view->m_ekf_view->insert("x", "predict_world", p.x()+1, predict_world_coordinate[0]);
//     x-measure
    p = m_view->m_ekf_view->getLastPoint("x", "measure");
    m_view->m_ekf_view->insert("x", "measure", p.x()+1, armor.pose.x);
//     x-predict
    p = m_view->m_ekf_view->getLastPoint("x", "predict");
    m_view->m_ekf_view->insert("x", "predict", p.x()+1, predict_camera_coordinate[0]);
//     x-predict_shoot
    p = m_view->m_ekf_view->getLastPoint("x", "predict_shoot");
    m_view->m_ekf_view->insert("x", "predict_shoot", p.x()+1, shoot_camera_coordinate[0]);
    // v_x-predict
    p = m_view->m_ekf_view->getLastPoint("v_x", "predict");
    m_view->m_ekf_view->insert("v_x", "predict", p.x()+1, predict_state[1]);
    // y-measure-word
    p = m_view->m_ekf_view->getLastPoint("y", "measure_world");
    m_view->m_ekf_view->insert("y", "measure_world", p.x()+1, armor.world_coordinate[1]);
    // y-predict-world
    p = m_view->m_ekf_view->getLastPoint("y", "predict_world");
    m_view->m_ekf_view->insert("y", "predict_world", p.x()+1, predict_world_coordinate[1]);
    // y-measure
    p = m_view->m_ekf_view->getLastPoint("y", "measure");
    m_view->m_ekf_view->insert("y", "measure", p.x()+1, armor.pose.y);
    // y-predict
    p = m_view->m_ekf_view->getLastPoint("y", "predict");
    m_view->m_ekf_view->insert("y", "predict", p.x()+1, predict_camera_coordinate[1]);
    // v_y-predict
    p = m_view->m_ekf_view->getLastPoint("v_y", "predict");
    m_view->m_ekf_view->insert("v_y", "predict", p.x()+1, predict_state[3]);
}

void ViewWork::viewEuler(const Eigen::Vector3d& imu, const Eigen::Vector3d& aim) const {
    int count;
    double yaw, pitch;
    for (const auto& [key, value]: {std::make_pair("receive imu", imu),
                                    std::make_pair("aim", aim)}) {
        count = m_view->m_imu_euler->getView("Euler")->getSeries(key)->count();
        yaw = value[0] * 180 / M_PI;
        pitch = value[1] * 180 / M_PI;
        if (count <= 0)
            m_view->m_imu_euler->getView("Euler")->getSeries(key)->append(yaw, pitch);
        else
            m_view->m_imu_euler->getView("Euler")->getSeries(key)->replace(0, yaw, pitch);
    }
}

void ViewWork::viewTimestamp(const uint64_t& camera_timestamp,
                             const uint64_t& imu_timestamp) {
    using Unit = std::chrono::milliseconds;
    using Clock = std::chrono::system_clock;
    QPointF p;
    p = m_view->m_timestamp_view->getLastPoint("Camera-IMU timestamp", "timestamp");
    auto c = static_cast<int64_t>(camera_timestamp);
    auto i = static_cast<int64_t>(imu_timestamp);
    auto now = static_cast<int64_t>(
            std::chrono::duration_cast<Unit>(Clock::now().time_since_epoch()).count());
    m_view->m_timestamp_view->insert(
            "Camera-IMU timestamp", "timestamp", p.x()+1, static_cast<double>(c - i));
    m_view->m_timestamp_view->insert(
            "Camera-IMU timestamp", "now-camera", p.x(), static_cast<double>(now - c));
    m_view->m_timestamp_view->insert(
            "Camera-IMU timestamp", "now-imu", p.x(), static_cast<double>(now - i));
}

void ViewWork::viewFaceAngle(float yaw, float pre_yaw) {
    QPointF p;
    p = m_view->m_face_angle_view->getLastPoint("yaw", "pose");
    m_view->m_face_angle_view->insert("yaw", "pose", p.x()+1, yaw);
    p = m_view->m_face_angle_view->getLastPoint("yaw", "predict");
    m_view->m_face_angle_view->insert("yaw", "predict", p.x()+1, pre_yaw);
}
}
