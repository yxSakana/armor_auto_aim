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
    httplib::Result result = m_client.Get("/test");
    if (result && result->status == 200) {
        DLOG(INFO) << "Connect server!";
    } else {
        DLOG(ERROR) << result.error();
    }
}

void PlotClientHttp::createWindowRequest(const nlohmann::json& json_data) {
    httplib::Result result = m_client.Post("/create_window", json_data.dump(), "application/json");
    if (result && result->status == 200) {
        DLOG(INFO) << fmt::format("Create window({}) successfully!", to_string(json_data["window_name"]));
    } else {
        DLOG(ERROR) << fmt::format("Failed to send data: {}", to_string(result.error()));
    }
}

void PlotClientHttp::puhBackDataRequest(const nlohmann::json& json_data) {
    httplib::Result result = m_client.Post("/add_data", json_data.dump(), "application/json");
    if (result && result->status == 200) {
        DLOG(INFO) << "Send data successfully!";
    } else {
        DLOG(ERROR) << fmt::format("Failed to send data: {}", to_string(result.error()));
    }
}
}
