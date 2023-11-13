/**
 * @projectName armor_auto_aiming
 * @file ekf_plot.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-09 21:22
 */

#include <plot_client_http/ekf_plot.h>

#include <google_logger/google_logger.h>

namespace armor_auto_aim::ekf_plot {
void lineSystemCreateWindowRequest(PlotClientHttp* plot_client_http) {
    nlohmann::json create_window_data = {
            { "window_name", "EKF - Line System" },
            { "rows", 1 },
            { "cols", 3 },
            { "multiple_axes", {
                {"00", {
                    { "type", "realtime_comparison" },
                    { "property", {
                        { "axes_title", "track armor x" },
                        { "data_name", "xa" },
                        { "data_unit", "m" }
                    }},
                }},
                {"01", {
                     { "type", "realtime_comparison" },
                     { "property", {
                       { "axes_title", "track armor y" },
                       { "data_name", "ya" },
                       { "data_unit", "m" }
                   }},
                }},
                {"02", {
                     { "type", "realtime_comparison" },
                     { "property", {
                       { "axes_title", "track armor y" },
                       { "data_name", "ya" },
                       { "data_unit", "m" }
                   }},
                }},
            } }
    };
    LOG(INFO) << create_window_data;
    plot_client_http->createWindowRequest(create_window_data);
}

void lineSystemPuhBackDataRequest(PlotClientHttp* plot_client_http, const Tracker& tracker) {
    if (tracker.state() == armor_auto_aim::TrackerStateMachine::State::Tracking ||
        tracker.state() == armor_auto_aim::TrackerStateMachine::State::TempLost) {
        Eigen::VectorXd measurement = tracker.measurement;
        const Eigen::VectorXd& target_state = tracker.getTargetSate();
        Eigen::Vector3d target_state_position(target_state(0), target_state(2), target_state(4));

        nlohmann::json json_data = {
                { "window_name", "EKF - Line System" },
                { "row", 0 }

        };
        for (int i = 0; i < measurement.size(); i++) {
            json_data["col"] = std::to_string(i);
            json_data["data"].push_back(nlohmann::json::array({measurement(i), target_state_position(i)}));
        }
//        LOG(INFO) << "Target State: " << target_state;
//        LOG(INFO) << fmt::format("measurement size: {}, json data: {}", measurement.size(), json_data.dump());
        plot_client_http->puhBackDataRequest(json_data);
    }
}
} // armor_auto_aim