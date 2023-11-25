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
                       { "axes_title", "track armor z" },
                       { "data_name", "za" },
                       { "data_unit", "m" }
                   }},
                }},
            } }
    };
//    DLOG(INFO) << create_window_data;
    plot_client_http->createWindowRequest(create_window_data);
    create_window_data["window_name"] = "Ekf - Yaw";
    create_window_data["cols"] = 2;
    create_window_data["multiple_axes"] = {
        {"00", {
            { "type", "realtime_comparison" },
            { "property", {
                { "axes_title", "PnP Yaw" },
                { "data_name", "yaw" },
                { "data_unit", "m" }
            }}
        }},
        {"01", {
            { "type", "realtime_waveform" },
            { "property", {
                { "axes_title", "PnP V_yaw" },
                { "data_name", "V_yaw" },
                { "data_unit", "°/s" },
                { "y_lim", nlohmann::json::array({-0.1, 0.1})}
            }}
        }}
    };
    plot_client_http->createWindowRequest(create_window_data);
}

void lineSystemUpdateDataRequest(PlotClientHttp* plot_client_http, const Tracker& tracker) {
    if (tracker.state() == armor_auto_aim::TrackerStateMachine::State::Tracking ||
        tracker.state() == armor_auto_aim::TrackerStateMachine::State::TempLost) {
        Eigen::VectorXd measurement = tracker.measurement;
        const Eigen::VectorXd& target_predict_state = tracker.getTargetPredictSate();
        Eigen::Vector3d target_predict_state_position(target_predict_state(0), target_predict_state(2), target_predict_state(4));

        nlohmann::json json_data = {
                { "window_name", "EKF - Line System" },
                { "row", 0 }
        };
        for (int i = 0; i < 3; i++) {
            json_data["col"] = std::to_string(i);
            json_data["data"] = nlohmann::json::array({measurement(i), target_predict_state_position(i)});
            plot_client_http->updateDateRequest(json_data);
        }

        json_data["window_name"] = "Ekf - Yaw";
        json_data["col"] = 0;
        json_data["data"] = nlohmann::json::array({measurement(3) * 180 / M_PI, target_predict_state(6) * 180 / M_PI});
        plot_client_http->updateDateRequest(json_data);

        json_data["col"] = 1;
        json_data["data"] = nlohmann::json::array({target_predict_state(7)});
        plot_client_http->updateDateRequest(json_data);
    }
}
} // armor_auto_aim