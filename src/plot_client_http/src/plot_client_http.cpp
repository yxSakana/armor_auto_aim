/**
 * @projectName socket
 * @file plot_client_http.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-07 20:37
 */

#include <plot_client_http/plot_client_http.h>

#include <google_logger/google_logger.h>

namespace armor_auto_aim {
PlotClientHttp::PlotClientHttp(const std::string& host, const int& port) \
        : m_client(host, port) {
    httplib::Result result = m_client.Get("/index");
    if (result && result->status == 200) {
        DLOG(INFO) << "Connect server!";
        m_is_connected = true;
    } else {
        DLOG(ERROR) << fmt::format("Unable to connect to sever: {}", to_string(result.error()));
        m_is_connected = false;
    }
}

void PlotClientHttp::createWindowRequest(const nlohmann::json& json_data) {
    if (m_is_connected) {
        httplib::Result result = m_client.Post("/create_window", json_data.dump(), "application/json");
        if (result && result->status == 200) {
            DLOG(INFO) << fmt::format("Create window({}) successfully!", to_string(json_data["window_name"]));
        } else {
            DLOG(ERROR) << fmt::format("Failed to send data: {}", to_string(result.error()));
        }
    }
}

void PlotClientHttp::updateDateRequest(const nlohmann::json& json_data) {
    if (m_is_connected) {
        httplib::Result result = m_client.Post("/update_data", json_data.dump(), "application/json");
        if (result && result->status == 200) {
            DLOG_EVERY_N(INFO, 100) << "Send data successfully!";
        } else {
            DLOG(ERROR) << fmt::format("Failed to send data: {}", to_string(result.error()));
        }
    }
}
}
