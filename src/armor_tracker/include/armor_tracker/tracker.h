/**
 * @projectName armor_auto_aim
 * @file track.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-03 19:29
 */

#ifndef ARMOR_AUTO_AIMING_ARMOR_TRACKER_TRACK_H
#define ARMOR_AUTO_AIMING_ARMOR_TRACKER_TRACK_H

#include <map>

#include <armor_tracker/extended_kalman_filter.h>
#include <armor_detector/interface.h>

namespace armor_auto_aim {
class Tracker;

class TrackerStateMachine {
public:
    enum class State {
        Lost,
        Detecting,
        Tracking,
        TempLost
    };

    TrackerStateMachine() =default;

    inline void initState() { m_state = State::Detecting; }

    [[nodiscard]] inline State state() const { return m_state; };

    [[nodiscard]] inline std::string stateString() const { return m_state_map.find(m_state)->second; }

    void update(bool detector_result);
private:
    State m_state = State::Lost;
    int m_detect_count = 0;
    int m_lost_count = 0;
    static constexpr int m_tracking_threshold = 10;
    static constexpr int m_lost_threshold = 40;

    std::map<State, std::string> m_state_map {
            { State::Lost, "Lost" },
            { State::Detecting, "Detecting" },
            { State::Tracking, "Tracking" },
            { State::TempLost, "TempLost" }
    };
};

class Tracker {
public:
    Tracker() =default;

    void initTracker(const Armors& armors);

    void updateTracker(const Armors& armors);

    [[nodiscard]] inline TrackerStateMachine::State state() const { return m_tracker_state_machine.state(); }

    [[nodiscard]] inline std::string stateString() const { return m_tracker_state_machine.stateString(); }

    inline bool isTracking() const {
        return m_tracker_state_machine.state() == TrackerStateMachine::State::Tracking ||
               m_tracker_state_machine.state() == TrackerStateMachine::State::TempLost;
    }

    double getLastYaw() const { return m_last_yaw; }

    [[nodiscard]] const Eigen::VectorXd& getTargetPredictSate() const { return m_target_predict_state; }

    std::shared_ptr<ExtendedKalmanFilter> ekf = nullptr;
    Armor tracked_armor;
    Eigen::VectorXd measurement;
private:
    static constexpr double m_MaxMatchDistance = 0.5;
    static constexpr double m_MaxMatchYaw = 0.1;  // FIXME: 修改为合理值

    Eigen::VectorXd m_target_predict_state;
    TrackerStateMachine m_tracker_state_machine;
    int m_tracked_id{};  // armor number
    double m_last_yaw = .0;

    void initEkf(const Armor& armor);

    void handleArmorJump(const Armor& same_id_armor);

    static double shortest_angular_distance(double from, double to);

    double correctYaw(const double yaw);

    static double normalize_radians(double rad) {
        double pi = std::atan(1) * 4;
        double result = std::fmod(rad, 2 * pi);
        if (result <= -pi)
            result += 2 * pi;
        else if (result > pi)
            result -= 2 * pi;
        return result;
    }

    Eigen::Vector3d getPositionFromState(const Eigen::VectorXd& x);
};
} // armor_auto_aim

#endif //ARMOR_AUTO_AIMING_ARMOR_TRACKER_TRACK_H
