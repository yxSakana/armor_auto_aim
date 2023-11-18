/**
 * @project_name auto_aim
 * @file pnp_view.h
 * @brief
 * @author yx
 * @data 2023-11-18 10:53:17
 */

#ifndef AUTO_AIM_PNP_VIEW_H
#define AUTO_AIM_PNP_VIEW_H

#include <armor_tracker/tracker.h>
#include <plot_client_http/plot_client_http.h>

namespace armor_auto_aim::pnp_view {
void pnpViewCreateWindowRequest(PlotClientHttp* plot_client_http);

void pnpViewUpdateDataRequest(PlotClientHttp* plot_client_http, const Tracker& tracker);
}

#endif //AUTO_AIM_PNP_VIEW_H
