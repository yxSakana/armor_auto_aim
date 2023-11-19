/**
 * @project_name auto_aim
 * @file pnp_view.cpp
 * @brief
 * @author yx
 * @data 2023-11-18 10:53:35
 */

#include <plot_client_http/pnp_view.h>
#include <google_logger/google_logger.h>
#include <solver/solver_interface.h>

namespace armor_auto_aim::pnp_view {
void pnpViewCreateWindowRequest(PlotClientHttp* plot_client_http) {
    nlohmann::json create_window_data = {
        { "window_name", "PnP View" },
        { "rows", 1 },
        { "cols", 1 },
        { "multiple_axes", {
            {"00", {
                { "type", "realtime_position" },
                { "property", {
                    { "axes_title", "track armor x" },
                    { "x_val_name", "yaw" },
                    { "x_val_unit", "°" },
                    { "y_val_name", "pitch" },
                    { "y_val_unit", "°" },
                    { "x_lim", nlohmann::json::array({-180, 180}) },
                    { "y_lim", nlohmann::json::array({-180, 180}) }
                }}
            }}
        } }
    };
    DLOG(INFO) << create_window_data;
    plot_client_http->createWindowRequest(create_window_data);
}

void pnpViewUpdateDataRequest(PlotClientHttp* plot_client_http, const Tracker& tracker) {
    if (tracker.state() == armor_auto_aim::TrackerStateMachine::State::Tracking ||
        tracker.state() == armor_auto_aim::TrackerStateMachine::State::TempLost) {
        nlohmann::json json_data = {
                { "window_name", "PnP View" },
                { "row", 0 },
                { "col", 0 },
        };
        solver::Pose tracked_armor_pose = tracker.tracked_armor.pose;
        json_data["data"] = nlohmann::json::array({tracked_armor_pose.yaw, tracked_armor_pose.pitch});
        plot_client_http->updateDateRequest(json_data);
    }
}
}
