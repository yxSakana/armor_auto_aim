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
import time
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
from plot.realtime_factory import RealtimeFactory
from ui.plot_ui import *
from ui.plot_ui import PlotUi

app = Flask(__name__)
k_FigurePool: Dict = {}


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
    figure: plt.Figure = plt.figure(figsize=(16, 10))
    grid_spec: plt.GridSpec = gridspec.GridSpec(rows, cols)
    k_FigurePool[window_name]["figure"] = figure
    k_FigurePool[window_name]["grid_spec"] = grid_spec
    for row in range(rows):
        for col in range(cols):
            current = k_FigurePool[window_name]["multiple_axes"][f"{row}{col}"]
            current["axes"] = RealtimeFactory.create_axes(figure, grid_spec, row, col, current)

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

    def update():
        start = time.time()
        k_FigurePool[window_name]["multiple_axes"][f"{row}{col}"]["axes"].update_data(new_data)
        end = time.time()
        print(f"update_data()-time: {end - start}")

        start = time.time()
        plt.savefig("./figure.jpg")
        end = time.time()
        print(f'plt.savefig("./figure.jpg")-time: {end - start}')

        start = time.time()
        plot_ui.set_plot_image("./figure.jpg")
        end = time.time()
        print(f'plot_ui.set_plot_image("./figure.jpg")-time: {end - start}')

    update()
    return f"Update data success: {window_name}-{row}:{col}", 200


@app.route("/test", methods=["GET"])
def test():
    return "Hello, World!", 200


def run():
    # plot_ui_factory.get_ui_sign.emit("test_new")
    app.run(host="127.0.0.1", port=12222)
    # plot_ui_factory.ui_factory["test_new"].show()
    # plot_ui.show()


if __name__ == "__main__":
    init_logger_configure()
    plt.subplots_adjust(hspace=1)
    q_app = QApplication(sys.argv)

    # plot_ui = PlotUi()
    # plot_ui_factory = PlotUiFactory()
    threading.Thread(target=run).start()
    plot_ui = PlotUi()
    plot_ui.show()

    q_app.exec_()
