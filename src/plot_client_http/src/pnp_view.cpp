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
        { "window_name", "Tracked Armor Translation - PNP" },
        { "rows", 1 },
        { "cols", 3 },
        { "multiple_axes", {
            {"00", {
                { "type", "realtime_waveform" },
                { "property", {
                    { "axes_title", "armor x" },
                    { "data_name", "x" },
                    { "data_unit", "m" },
                    { "y_lim", nlohmann::json::array({-5, 5}) }
                }}
            }},
            {"01", {
                { "type", "realtime_waveform" },
                { "property", {
                    { "axes_title", "armor y" },
                    { "data_name", "y" },
                    { "data_unit", "m" },
                    { "y_lim", nlohmann::json::array({-5, 5}) }
                }}
            }},
            {"02", {
                { "type", "realtime_waveform" },
                { "property", {
                    { "axes_title", "armor z" },
                    { "data_name", "z" },
                    { "data_unit", "m" },
                    { "y_lim", nlohmann::json::array({-5, 5}) }
                }}
            }}
        }}
    };
    plot_client_http->createWindowRequest(create_window_data);
}

void pnpViewUpdateDataRequest(PlotClientHttp* plot_client_http, const Tracker& tracker) {
    if (tracker.state() == armor_auto_aim::TrackerStateMachine::State::Tracking ||
        tracker.state() == armor_auto_aim::TrackerStateMachine::State::TempLost) {
        nlohmann::json json_data = {
                { "window_name", "Tracked Armor Translation - PNP" },
                { "row", 0 },
                { "col", 0 },
        };
        solver::Pose tracked_armor_pose = tracker.tracked_armor.pose;

        json_data["data"] = nlohmann::json::array({tracked_armor_pose.x});
        plot_client_http->updateDateRequest(json_data);

        json_data["col"] = 1;
        json_data["data"] = nlohmann::json::array({tracked_armor_pose.y});
        plot_client_http->updateDateRequest(json_data);

        json_data["col"] = 2;
        json_data["data"] = nlohmann::json::array({tracked_armor_pose.z});
        plot_client_http->updateDateRequest(json_data);
    }
}
}
