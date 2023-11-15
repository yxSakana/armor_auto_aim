/**
 * @projectName armor_auto_aiming
 * @file ekf_plot.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-09 21:22
 */

#ifndef ARMOR_AUTO_AIMING_EKF_PLOT_H
#define ARMOR_AUTO_AIMING_EKF_PLOT_H

#include <armor_tracker/tracker.h>
#include <plot_client_http/plot_client_http.h>

namespace armor_auto_aim::ekf_plot {
void lineSystemCreateWindowRequest(PlotClientHttp* plot_client_http);

void lineSystemUpdateDataRequest(PlotClientHttp* plot_client_http, const Tracker& tracker);
} // armor_auto_aim

#endif //ARMOR_AUTO_AIMING_EKF_PLOT_H
