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
from typing import Dict, List

from loguru import logger
from PyQt5.Qt import QApplication
from flask import Flask, request
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

from units.log_configure import init_logger_configure
from plot.real_time_comparison_image import RealTimeComparisonImage
from ui.plot_ui import PlotUi

app = Flask(__name__)
k_FigurePool: Dict = {
    "real_time_comparison": {}
}


@app.route("/create_window", methods=["POST"])
def create_window():
    if request.is_json:
        data = request.get_json()
        figure_type = data["figure_type"]
        window_name = data["window_name"]
        if k_FigurePool.get(figure_type) is None:
            return f"Failed: figure_type {figure_type}", 201

        if figure_type == "real_time_comparison":
            if k_FigurePool[figure_type].get(window_name) is None:
                figure = plt.figure(figsize=(16, 10))
                grid_spec = gridspec.GridSpec(data["rows"], data["cols"])
                k_FigurePool[figure_type][window_name] = {}
                k_FigurePool[figure_type][window_name]["figure"] = figure
                k_FigurePool[figure_type][window_name]["grid_spec"] = grid_spec
                k_FigurePool[figure_type][window_name]["master"]: List = []

                index = 0
                for row in range(int(data["rows"])):
                    for col in range(int(data["cols"])):
                        k_FigurePool[figure_type][window_name]["master"].append(
                            RealTimeComparisonImage(figure, grid_spec, row, col,
                                                    data["title"][index], data["val_name"][index],
                                                    data["val_unit"][index], data["val_label"][index])
                        )
                        index += 1
        logger.info(f"create window successful! {figure_type}:{window_name}")
    return "", 200


@app.route("/add_data", methods=["POST"])
def add_data():
    if not request.is_json:
        return f"Failed: data is not json", 202

    data = request.get_json()
    figure_type = data["figure_type"]
    window_name = data["window_name"]
    if k_FigurePool.get(figure_type) is None:
        return f"Failed: figure_type {figure_type}", 201
    if k_FigurePool[figure_type].get(window_name) is None:
        return f"Failed: window_name is None: {window_name}", 202

    for index, master in enumerate(k_FigurePool[figure_type][window_name]["master"]):
        master.push_back(data["data"][index])
    plt.savefig("figure.jpg")
    plot_ui.set_plot_image("figure.jpg")
    return "OK", 200


@app.route("/test", methods=["GET"])
def test():
    return "Hello, World!", 200


def run():
    app.run(host="127.0.0.1", port=12222)


if __name__ == "__main__":
    init_logger_configure()
    plt.subplots_adjust(hspace=1)
    q_app = QApplication(sys.argv)

    plot_ui = PlotUi()
    threading.Thread(target=run).start()
    plot_ui.show()

    q_app.exec_()
