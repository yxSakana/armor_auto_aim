/**
 * @projectName armor_auto_aiming
 * @file ekf_plot.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-09 21:22
 */
 
#include <fstream>

#include <plot_client_http/ekf_plot.h>

#include <google_logger/google_logger.h>

namespace armor_auto_aim::ekf_plot {
void lineSystemCreateWindowRequest(PlotClientHttp* plot_client_http) {
    std::ifstream json_file("../config/view/ekf_view.json");
    nlohmann::json json_data = nlohmann::json::parse(json_file);

    nlohmann::json create_window_data;
    for (const auto& key: {"Predict Translation - EKF",  "Camera Yaw-Pitch"}) {
        create_window_data = json_data[key];
        DLOG(INFO) << create_window_data;
        plot_client_http->createWindowRequest(create_window_data);
    }
}

void lineSystemUpdateDataRequest(PlotClientHttp* plot_client_http, const Tracker& tracker) {
    if (tracker.state() == armor_auto_aim::TrackerStateMachine::State::Tracking ||
        tracker.state() == armor_auto_aim::TrackerStateMachine::State::TempLost) {

        Eigen::VectorXd measurement = tracker.measurement;
        const Eigen::VectorXd& target_predict_state = tracker.getTargetPredictSate();
        // Predict Translation - EKF
        nlohmann::json json_data = {
                { "window_name", "Predict Translation - EKF" },
                { "row", 0 }
        };
        // translation
        for (int i = 0; i < 6; i += 2) {
            json_data["col"] = std::to_string(i / 2);
            json_data["data"] = nlohmann::json::array({measurement(i / 2), target_predict_state(i)});
            plot_client_http->asyncUpdateDateRequest(json_data);
        }
        // translation v
        json_data["row"] = 1;
        for (int i = 1; i < 7; i += 2) {
            json_data["col"] = std::to_string(int(i / 2));
            json_data["data"] = nlohmann::json::array({target_predict_state[i]});
            plot_client_http->asyncUpdateDateRequest(json_data);
        }
        // Camera Yaw-Pitch
        json_data = {
                { "window_name", "Camera Yaw-Pitch" },
                { "row", 0 }
        };
        // Yaw
        json_data["col"] = 0;
        json_data["data"] = nlohmann::json::array({std::atan2(target_predict_state[0],
                                                              target_predict_state[4]) * 180 / M_PI});
        plot_client_http->asyncUpdateDateRequest(json_data);
        // Pitch
        json_data["col"] = 1;
        json_data["data"] = nlohmann::json::array({std::atan2(target_predict_state[2],
                                                              target_predict_state[4]) * 180 / M_PI});
        plot_client_http->asyncUpdateDateRequest(json_data);
    }
}
} // armor_auto_aim