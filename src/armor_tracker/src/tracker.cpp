/**
 * @projectName armor_auto_aim
 * @file track.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-03 19:29
 */

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
            ++m_detect_count;
            if (m_detect_count > m_tracking_threshold) {
                m_state = State::Tracking;
            }
        } else {
            m_detect_count = 0;
            m_state = State::Lost;
        }
    } else if (m_state == State::Tracking) {
        if (!detector_result) {
            m_state = State::TempLost;
        }
    } else if (m_state == State::TempLost) {
        if (detector_result) {
            m_lost_count = 0;
            m_state = State::Tracking;
        } else {
            ++m_lost_count;
            if (m_lost_count > m_lost_threshold) {
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
        if (armor.pose.z < min_distance) {
            min_distance = armor.pose.z;
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
        double min_position_difference = DBL_MAX;
        double yaw_difference = DBL_MAX;
        Eigen::Vector3d measurement_position_vec;
        Eigen::Vector3d predicted_position_vec(
                m_target_predict_state(0),
                m_target_predict_state(2),
                m_target_predict_state(4));
        for (const auto& armor: armors) {
            if (armor.number == m_tracked_id) {
                same_id_armor = &armor;
                same_id_armor_count++;
                measurement_position_vec = Eigen::Vector3d(armor.pose.x, armor.pose.y, armor.pose.z);
                double position_difference = (predicted_position_vec - measurement_position_vec).norm();
                if (position_difference < min_position_difference) {
                    min_position_difference = position_difference;
                    tracked_armor = armor;
                }
            }
        }
//        DLOG(INFO) << "min_position_diff: " << min_position_difference;
        // 后验及装甲板跳变处理
        if (min_position_difference < m_MaxMatchDistance) {
//             TODO: 是否需要使用shortestAngularDistance 对 yaw 进行处理
            is_matched = true;
            measurement = Eigen::Vector4d(tracked_armor.pose.x, tracked_armor.pose.y,
                                          tracked_armor.pose.z, tracked_armor.pose.yaw);
            m_target_predict_state = ekf->predict(measurement);
        } else if (same_id_armor_count == 1) {
            LOG(WARNING) << "armor jump";
        }
    }
    // update
    m_tracker_state_machine.update(is_matched);
}

void Tracker::initEkf(const Armor& armor) {
    double xa = armor.pose.x;
    double ya = armor.pose.y;
    double za = armor.pose.z;
    double yaw = armor.pose.yaw;

    m_target_predict_state = Eigen::VectorXd::Zero(8);
    m_target_predict_state << xa, 0, ya, 0, za, 0, yaw, 0;

    ekf->setState(m_target_predict_state);
}
} // armor_auto_aim