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
            { "figure_type", "real_time_comparison" },
            { "window_name", "EKF - Line System" },
            { "rows", 1 },
            { "cols", 3 },
            { "title", {
               "x_c", "y_c", "z_a",
            }},
            { "val_name", {
                "x_c", "y_c", "z_a",
            }},
            { "val_unit", {
               "m",  "m",  "m",
            }}
    };
    nlohmann::json val_label = nlohmann::json::array();
    for (int i = 0; i < 6; i++)
        val_label.push_back(nlohmann::json::array({"measurement", "prediction"}));
    create_window_data["val_label"] = val_label;

    plot_client_http->createWindowRequest(create_window_data);
}

void lineSystemPuhBackDataRequest(PlotClientHttp* plot_client_http, const Tracker& tracker) {
    if (tracker.state() == armor_auto_aim::TrackerStateMachine::State::Tracking ||
        tracker.state() == armor_auto_aim::TrackerStateMachine::State::TempLost) {
        nlohmann::json json_data = {
                {"figure_type", "real_time_comparison"},
                {"window_name", "EKF - Line System"}
        };
        Eigen::VectorXd measurement = tracker.measurement;
        const Eigen::VectorXd& target_state = tracker.getTargetSate();
        Eigen::Vector3d target_state_tmp(target_state(0), target_state(2), target_state(4));
        LOG(INFO) << "Target State: " << target_state;
        for (int i = 0; i < measurement.size(); i++)
            json_data["data"].push_back(nlohmann::json::array({measurement(i), target_state_tmp(i)}));
        LOG(INFO) << fmt::format("measurement size: {}, json data: {}", measurement.size(), json_data.dump());
        plot_client_http->puhBackDataRequest(json_data);
    }
}
} // armor_auto_aim