/**
 * @project_name auto_aim
 * @file yaw_pitch_view.cpp
 * @brief
 * @author yx
 * @data 2023-11-24 14:45:50
 */

#include <plot_client_http/yaw_pitch_view.h>
#include <google_logger/google_logger.h>

namespace armor_auto_aim::yaw_pitch_view {
void yawPitchViewCreateWindowRequest(PlotClientHttp* plot_client_http) {
        nlohmann::json create_window_data = {
        { "window_name", "Yaw-Pitch-View" },
        { "rows", 1 },
        { "cols", 3 },
        {"multiple_axes", {
            {"00", {
                { "type", "realtime_position" },
                { "property", {
                    { "axes_title", "Yaw-Pitch" },
                    { "x_val_name", "yaw" },
                    { "x_val_unit", "°" },
                    { "y_val_name", "pitch" },
                    { "y_val_unit", "°" },
                    { "x_lim", nlohmann::json::array({-180, 180}) },
                    { "y_lim", nlohmann::json::array({-180, 180}) }
                }}
            }},
            {"01", {
                {"type", "realtime_waveform"},
                {"property", {
                    { "axes_title", "Yaw" },
                    { "data_name", "Yaw" },
                    { "data_unit", "°" },
                    { "y_lim", nlohmann::json::array({-180, 180}) }
                }}
            }},
            {"02", {
                {"type", "realtime_waveform"},
                {"property", {
                    { "axes_title", "Pitch" },
                    { "data_name", "Pitch" },
                    { "data_unit", "°" },
                    { "y_lim", nlohmann::json::array({-180, 180}) }
                }}
            }}
        }
        }
    };
    plot_client_http->createWindowRequest(create_window_data);
}

void yawPitchViewUpdateDataRequest(PlotClientHttp* plot_client_http, const CommunicateProtocol& communicate_protocol) {
    nlohmann::json json_data = {
            { "window_name", "Yaw-Pitch-View" },
            { "row", 0 },
    };
    json_data["col"] = 0;
    json_data["data"] = nlohmann::json::array({communicate_protocol.yaw, communicate_protocol.pitch});
    plot_client_http->updateDateRequest(json_data);

    json_data["col"] = 1;
    json_data["data"] = nlohmann::json::array({communicate_protocol.yaw});
    plot_client_http->updateDateRequest(json_data);

    json_data["col"] = 2;
    json_data["data"] = nlohmann::json::array({communicate_protocol.pitch});
    plot_client_http->updateDateRequest(json_data);
}
}
