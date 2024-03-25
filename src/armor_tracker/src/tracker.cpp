/**
 * @projectName armor_auto_aim
 * @file track.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-03 19:29
 */

#define USE_COS
//#define USE_SIN

#include <armor_tracker/tracker.h>

#include <glog/logging.h>

namespace armor_auto_aim {
void TrackerStateMachine::update(bool detector_result) {
    if (m_state == State::Lost) {
        if (!detector_result) {
            m_detect_count = 0;
            m_lost_count = 0;
        }
    } else if (m_state == State::Detecting) {
        if (detector_result) {
            if (++m_detect_count > m_tracking_threshold) m_state = State::Tracking;
        } else {
            m_detect_count = 0;
            m_state = State::Lost;
        }
    } else if (m_state == State::Tracking) {
        if (!detector_result) m_state = State::TempLost;

    } else if (m_state == State::TempLost) {
        if (detector_result) {
            m_lost_count = 0;
            m_state = State::Tracking;
        } else {
            if (++m_lost_count > m_lost_threshold) {
                m_lost_count = 0;
                m_state = State::Lost;
            }
        }
    }
}

void Tracker::initTracker(const Armors& armors) {
    LOG_IF(ERROR, ekf == nullptr) << "ekf is nullptr";
    if (armors.empty() || ekf == nullptr)
        return;
    // 选择要跟踪的装甲板(优先选择距离最近的)(en: Select tracked armor)
    double min_distance = DBL_MAX;
    tracked_armor = armors[0];
    for (const auto& armor: armors) {
        // TODO: 是否需要改为 (z, x) 与 image_point(x, y)的距离
        if (armor.world_coordinate[2] < min_distance) {
            min_distance = armor.world_coordinate[2];
            tracked_armor = armor;
        }
    }
    // initialization
    initEkf(tracked_armor);
    m_tracker_state_machine.initState();
    m_tracked_id = tracked_armor.number;
}

void Tracker::updateTracker(const Armors& armors) {
    // prior
    bool is_matched = false;
    const Armor* same_id_armor;
    int same_id_armor_count = 0;
    m_target_predict_state = ekf->update();
    // 寻找tracked装甲板
    if (!armors.empty()) {
//        m_target_predict_state = ekf->update();
        double min_position_difference = DBL_MAX;
        double yaw_difference = DBL_MAX;
        Eigen::Vector3d measurement_position_vec{};
        Eigen::Vector3d predicted_position_vec(m_target_predict_state(0),
                                               m_target_predict_state(2),
                                               m_target_predict_state(4));
        for (const auto& armor: armors) {
            if (armor.number == m_tracked_id) {
               same_id_armor = &armor;
               same_id_armor_count++;
            }
            measurement_position_vec = armor.world_coordinate;
            double position_difference = (predicted_position_vec - measurement_position_vec).norm();
            if (position_difference < min_position_difference) {
                min_position_difference = position_difference;
                yaw_difference = std::abs(normalize_radians(m_target_predict_state[6]) - correctYaw(armor.pose.yaw));
                tracked_armor = armor;
            }
        }
        // 后验及装甲板跳变处理
        if (min_position_difference < m_MaxMatchDistance &&
            yaw_difference < m_MaxMatchYaw) {
//             TODO: 是否需要使用shortestAngularDistance 对 yaw 进行处理
            is_matched = true;
            measurement = Eigen::Vector4d(tracked_armor.world_coordinate[0], tracked_armor.world_coordinate[1],
                                          tracked_armor.world_coordinate[2], correctYaw(tracked_armor.pose.yaw));
            m_target_predict_state = ekf->predict(measurement);
        } else if (same_id_armor_count == 1 && yaw_difference > m_MaxMatchYaw) {
            handleArmorJump(*same_id_armor);
        } else {
            LOG_IF(WARNING, yaw_difference > m_MaxMatchYaw)
                            << fmt::format("No matched armor, because yaw: {} > {}",
                                           yaw_difference, m_MaxMatchYaw);
            LOG_IF(WARNING, min_position_difference > m_MaxMatchDistance)
                << fmt::format("No matched armor, because distance: {} > {}",
                               min_position_difference, m_MaxMatchDistance);
        }
    }
    if (m_target_predict_state(8) < 0.12) {
        m_target_predict_state(8) = 0.12;
        ekf->setState(m_target_predict_state);
    } else if (m_target_predict_state(8) > 0.4) {
        m_target_predict_state(8) = 0.4;
        ekf->setState(m_target_predict_state);
    }
    // update
    m_tracker_state_machine.update(is_matched);
}

void Tracker::initEkf(const Armor& armor) {
    double xa = armor.world_coordinate[0];
    double ya = armor.world_coordinate[1];
    double za = armor.world_coordinate[2];
    m_last_yaw = 0.0;
    double yaw = correctYaw(armor.pose.yaw);
    double r = 0.2;
    m_target_predict_state = Eigen::VectorXd::Zero(9);
#ifdef USE_SIN
    m_target_predict_state << xa + r*sin(yaw), 0, ya + r*cos(yaw), 0, za, 0, yaw, 0, r;
#endif
#ifdef USE_COS
    m_target_predict_state << xa + r*cos(yaw), 0, ya + r*sin(yaw), 0, za, 0, yaw, 0, r;
#endif
    Eigen::Matrix<double, 9, 9> p0;
    double p = 1000;
    //  xa  vxa  ya  vya  za  vza  yaw v_yaw  r
    p0 << p,  0,   0,  0,  0,   0,   0,  0,   0, // xa
          0,  p,   0,  0,  0,   0,   0,  0,   0, // vxa
          0,  0,   p,  0,  0,   0,   0,  0,   0, // ya
          0,  0,   0,  p,  0,   0,   0,  0,   0, // vya
          0,  0,   0,  0,  p,   0,   0,  0,   0, // za
          0,  0,   0,  0,  0,   p,   0,  0,   0, // vza
          0,  0,   0,  0,  0,   0,   p,  0,   0, // yaw
          0,  0,   0,  0,  0,   0,   0,  p,   0, // v_yaw
          0,  0,   0,  0,  0,   0,   0,  0,   p; // r
    ekf->initEkf(m_target_predict_state, p0);
}

void Tracker::handleArmorJump(const armor_auto_aim::Armor& same_id_armor) {
    // FIXME
    double x   = same_id_armor.world_coordinate[0],
           y   = same_id_armor.world_coordinate[1],
           z   = same_id_armor.world_coordinate[2],
           yaw = same_id_armor.pose.yaw,
           r   = m_target_predict_state[8];
#ifdef USE_SIN
    m_target_predict_state << x + r*sin(yaw), 0, y + r*cos(yaw), 0, z, 0, yaw, 0, r;
#endif
#ifdef USE_COS
    m_target_predict_state << x + r*cos(yaw), 0, y + r*sin(yaw), 0, z, 0, yaw, 0, r;
#endif
    ekf->setState(m_target_predict_state);
}

double Tracker::shortest_angular_distance(double from, double to) {
    double angle = to - from;
    const double result = fmod(angle + M_PI, 2.0*M_PI);
    return (result <= 0.0)? result + M_PI: result - M_PI;
}

static double adjustAngle(double angle) {
    while (angle < 0) {
        angle += 2 * M_PI;
    }
    while (angle >= 2 * M_PI) {
        angle -= 2 * M_PI;
    }
    return angle;
}

double Tracker::correctYaw(const double yaw) {
    m_last_yaw = yaw;
    return yaw;
//    LOG_EVERY_T(INFO, 1) << fmt::format("yaw: {}; last: {}; shortest: {}", yaw, m_last_yaw, shortest_angular_distance(m_last_yaw, yaw));
//    return adjustAngle(yaw);
    m_last_yaw = m_last_yaw + shortest_angular_distance(m_last_yaw, adjustAngle(yaw));
    return m_last_yaw;
}

Eigen::Vector3d Tracker::getPositionFromState(const Eigen::VectorXd& x) {
    double xc = x[0], yc = x[1], zc = x[2];
    double yaw = x[6], r = x[7];
    double ya = yc - r*cos(yaw);
    double xa = xc - r*sin(yaw);
    return {xa, ya, zc};
}
} // armor_auto_aim