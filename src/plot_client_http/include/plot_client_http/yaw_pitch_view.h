/**
 * @project_name auto_aim
 * @file yaw_pitch_view.h
 * @brief
 * @author yx
 * @data 2023-11-24 14:45:41
 */

#ifndef AUTO_AIM_YAW_PITCH_VIEW_H
#define AUTO_AIM_YAW_PITCH_VIEW_H

#include <serial_port/communicate_protocol.h>
#include <plot_client_http/plot_client_http.h>

namespace armor_auto_aim::yaw_pitch_view {
void yawPitchViewCreateWindowRequest(PlotClientHttp* plot_client_http);

void yawPitchViewUpdateDataRequest(PlotClientHttp* plot_client_http, const CommunicateProtocol& communicate_protocol);
}

#endif //AUTO_AIM_YAW_PITCH_VIEW_H
