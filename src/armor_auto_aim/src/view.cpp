/**
 * @project_name auto_aim
 * @file view.cpp
 * @brief
 * @author yx
 * @date 2023-12-24 18:04:45
 */

#include <armor_auto_aim/view.h>
#include <debug_toolkit/draw_package.h>

namespace armor_auto_aim {
ViewWork::ViewWork(QObject* parent)
        : QObject(parent),
          m_view(new view::View) {
    qRegisterMetaType<Eigen::Vector3d>("Eigen::Vector3d");
    qRegisterMetaType<uint64_t>("uint64_t");
    for (const auto& k: {"x", "v_x", "y", "v_y"})
        m_view->m_ekf_view->get(k)->setShowNumber(300);
    m_view->m_timestamp_view->get("Camera-IMU timestamp")->setShowNumber(100);
}

void ViewWork::show() {
    m_view->m_ekf_view->show();
    m_view->m_timestamp_view->show();
}

void ViewWork::showFrame(const cv::Mat& frame,
                         const std::vector<Armor>& armors, const armor_auto_aim::Tracker& tracker,
                         const double& fps, const uint64& timestamp, const float& dt) {
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

void ViewWork::viewTimestamp(const uint64& camera_timestamp,
                             const uint64& imu_timestamp) {
    QPointF p;
    p = m_view->m_timestamp_view->getLastPoint("Camera-IMU timestamp", "timestamp");
    auto c = static_cast<int64_t>(camera_timestamp);
    auto i = static_cast<int64_t>(imu_timestamp);
    m_view->m_timestamp_view->insert(
            "Camera-IMU timestamp", "timestamp", p.x()+1, static_cast<double>(c - i));
}
}
