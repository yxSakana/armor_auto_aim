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

import matplotlib
from loguru import logger
from PyQt5.Qt import QApplication
from flask import Flask, request
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.backends.qt_compat import QtWidgets
from matplotlib.backends.backend_qtagg import FigureCanvas

from units.log_configure import init_logger_configure
from custom_axes.realtime_comparison import RealtimeComparisonAxesProperty, RealtimeComparisonAxes
from custom_axes.realtime_position import RealtimePositionAxesProperty, RealtimePositionAxes
from custom_axes.realtime_factory import RealtimeFactory
from ui.figure_uii import FigureCanvasUiFactory, FigureCanvasUi
from ui.figure_uii import FigureCanvasUi

app = Flask(__name__)
k_FigurePool: Dict = {}


@app.route("/create_window", methods=["POST"])
def create_window():
    global k_FigurePool

    if not request.is_json:
        return "No Json Data", 1

    data: Dict = request.get_json()
    window_name: str = data.pop("window_name")
    if window_name in k_FigurePool.keys():
        return "Existed!", 200,

    k_FigurePool[window_name] = data.copy()
    figure_canvas_ui_factory.create_ui_sign.emit(window_name, k_FigurePool)
    pprint(k_FigurePool)
    return f"Create window success: {window_name}", 200


@app.route("/add_data", methods=["POST"])
def add_data():
    global k_FigurePool

    if not request.is_json:
        return f"Failed: data is not json", 202

    data = request.get_json()
    window_name = data["window_name"]
    row = data["row"]
    col = data["col"]
    new_data = data["data"]

    logger.info(f"new_data: {row}:{col} -- {new_data}")
    k_FigurePool[window_name]["multiple_axes"][f"{row}{col}"]["axes"].update_data(new_data)
    figure_canvas_ui_factory.update_ui_sign.emit(k_FigurePool[window_name]["figure_canvas"])
    # k_FigurePool[window_name]["figure_canvas"].draw()

    return f"Update data success: {window_name}-{row}:{col}", 200


@app.route("/test", methods=["GET"])
def test():
    return "Hello, World!", 200


def run():
    app.run(host="127.0.0.1", port=12222)


if __name__ == "__main__":
    init_logger_configure()
    matplotlib.use('Qt5Agg')
    q_app = QtWidgets.QApplication(sys.argv)

    figure_canvas_ui_factory = FigureCanvasUiFactory()
    threading.Thread(target=run).start()

    q_app.exec_()
