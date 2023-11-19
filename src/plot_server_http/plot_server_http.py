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
import concurrent.futures
from typing import Dict

import matplotlib
from loguru import logger
from flask import Flask, request
from matplotlib.backends.qt_compat import QtWidgets

import configure_logger
from ui.figure_uii import FigureCanvasUiFactory

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
    return f"Create window success: {window_name}", 200


@app.route("/update_data", methods=["POST"])
def update_data():
    global k_FigurePool

    if not request.is_json:
        return f"Failed: data is not json", 202
    data = request.get_json()
    window_name = data["window_name"]
    row = data["row"]
    col = data["col"]
    new_data = data["data"]

    # def update_data_func():
    #     k_FigurePool[window_name]["multiple_axes"][f"{row}{col}"]["axes"].update_data(new_data)
    # with concurrent.futures.ThreadPoolExecutor() as executor:
    #     executor.submit(update_data_func)
    k_FigurePool[window_name]["multiple_axes"][f"{row}{col}"]["axes"].update_data(new_data)
    return f"Update data success: {window_name}-{row}:{col}", 200


@app.route("/index", methods=["GET"])
def index():
    return "Hello, World!", 200


def run():
    app.run(host="127.0.0.1", port=12222)


if __name__ == "__main__":
    app.config["LOGURU_CONFIG"] = {
        "handlers": [
            {
                "sink": "logs/app.log",
                "level": "INFO",
                "rotation": "1 week"
            },
        ]
    }
    logger.info("-> [INFO] <- ^_^")
    # logger.configure(**app.config["LOGURU_CONFIG"])
    app.logger.disabled = True

    matplotlib.use('Qt5Agg')
    q_app = QtWidgets.QApplication(sys.argv)

    figure_canvas_ui_factory = FigureCanvasUiFactory()
    threading.Thread(target=run).start()

    q_app.exec_()
