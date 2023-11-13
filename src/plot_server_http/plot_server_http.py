# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file plot_server_http.py
  @brief 
 
  @author yx 
  @date 2023-11-05 10:33
"""

import sys
import threading
from pprint import pprint
from typing import Dict, List, Union

from loguru import logger
from PyQt5.Qt import QApplication
from flask import Flask, request
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

from units.log_configure import init_logger_configure
from plot.realtime_comparison import RealtimeComparisonAxesProperty, RealtimeComparisonAxes
from plot.realtime_position import RealtimePositionAxesProperty, RealtimePositionAxes
from ui.plot_ui import PlotUi

app = Flask(__name__)
k_FigurePool: Dict = {
}


@app.route("/create_window", methods=["POST"])
def create_window():
    if not request.is_json:
        return "No Json Data", 1

    data: Dict = request.get_json()
    window_name = data.pop("window_name")
    if window_name in k_FigurePool.keys():
        return "Existed!", 200,

    k_FigurePool[window_name] = data.copy()
    rows = int(k_FigurePool[window_name]["rows"])
    cols = int(k_FigurePool[window_name]["cols"])
    figure: plt.Figure = k_FigurePool[window_name].get("figure", plt.Figure())
    grid_spec: plt.GridSpec = k_FigurePool[window_name].get("grid_spec", gridspec.GridSpec(rows, cols))
    k_FigurePool[window_name]["figure"] = figure
    k_FigurePool[window_name]["grid_spec"] = grid_spec
    for row in range(rows):
        for col in range(cols):
            current = k_FigurePool[window_name]["multiple_axes"][f"{row}{col}"]
            current["axes"] = create_axes(figure, grid_spec, row, col, current)

    pprint(k_FigurePool)
    return f"Create window success: {window_name}", 200


@app.route("/add_data", methods=["POST"])
def add_data():
    if not request.is_json:
        return f"Failed: data is not json", 202

    data = request.get_json()
    window_name = data["window_name"]
    row = data["row"]
    col = data["col"]
    new_data = data["data"]

    k_FigurePool[window_name]["multiple_axes"][f"{row}{col}"]["axes"].update_data(new_data)
    plt.savefig("./figure.jpg")
    plot_ui.set_plot_image("./figure.jpg")
    return f"Update data success: {window_name}-{row}:{col}", 200


@app.route("/test", methods=["GET"])
def test():
    return "Hello, World!", 200


def create_axes(_figure: plt.Figure, _grid_spec: plt.GridSpec, _row: int, _col: int,
                _axes_property: Dict) -> Union[RealtimeComparisonAxes]:
    if _axes_property["type"] == "realtime_comparison":
        axes_title = _axes_property["property"]["axes_title"]
        data_name = _axes_property["property"]["data_name"]
        data_unit = _axes_property["property"]["data_unit"]
        axes = RealtimeComparisonAxes.create_axes(_figure, _grid_spec, _row, _col)
        axes_property = RealtimeComparisonAxesProperty(axes_title, data_name, data_unit)
        return RealtimeComparisonAxes(axes, axes_property)
    else:
        logger.error(f"Unknown type{_axes_property['type']}")


def run():
    app.run(host="127.0.0.1", port=12222)


if __name__ == "__main__":
    init_logger_configure()
    plt.subplots_adjust(hspace=1)
    q_app = QApplication(sys.argv)

    threading.Thread(target=run).start()
    plot_ui = PlotUi()
    plot_ui.show()

    q_app.exec_()
