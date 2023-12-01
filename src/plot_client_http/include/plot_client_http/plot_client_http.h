/**
 * @projectName socket
 * @file plot_client_http.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-07 20:37
 */

#ifndef ARMOR_AUTO_AIM_PLOT_CLIENT_HTTP_H
#define ARMOR_AUTO_AIM_PLOT_CLIENT_HTTP_H

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <thread_pool/thread_pool.h>

namespace armor_auto_aim {
class PlotClientHttp {
public:
    explicit PlotClientHttp(const std::string& host="127.0.0.1", const int& port=12222);

    void createWindowRequest(const nlohmann::json& json_data);

    void updateDateRequest(const nlohmann::json& json_data);

    void asyncUpdateDateRequest(const nlohmann::json& json_data);

    [[nodiscard]] inline bool isConnected() const { return m_is_connected; };
private:
    httplib::Client m_client;
    bool m_is_connected = false;
    ThreadPool m_request_thread_pool;
};
}

#endif //ARMOR_AUTO_AIM_PLOT_CLIENT_HTTP_H
