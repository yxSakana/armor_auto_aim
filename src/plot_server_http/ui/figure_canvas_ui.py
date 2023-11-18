# -*- coding: utf-8 -*-
"""  
  @project_name plot_server_http
  @file figure_canvas_ui.py
  @brief
  @author yx
  @data 2023-11-17 21:22:29
"""
import random
import sys
import threading
import time
from typing import List, Any, Tuple, Dict

from loguru import logger
from matplotlib.backends.backend_qtagg import FigureCanvas
from matplotlib.backends.backend_qtagg import \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.backends.qt_compat import QtWidgets
from matplotlib.figure import Figure
from mpl_toolkits import axisartist
from PyQt5 import Qt
from PyQt5.QtWidgets import QWidget

from custom_axes.realtime_interface import RealtimeAxesProperty
from custom_axes.realtime_comparison import RealtimeComparisonAxes, RealtimeComparisonAxesProperty


class FigureCanvasUi(QtWidgets.QMainWindow):
    __type: List[str] = ["default", "realtime_comparison", "realtime_position"]

    def __init__(self, _title: str = "title"):
        super().__init__()
        self._main = QtWidgets.QWidget()
        self.setWindowTitle(_title)
        self.setCentralWidget(self._main)
        self.layout = QtWidgets.QHBoxLayout(self._main)

        self.figure_canvas_axes: List = []

    def add_figure_canvas(self, _type: str, _property: Any):  # TODO: 改成信号触发
        if _type in FigureCanvasUi.__type:
            figure_canvas = FigureCanvas(Figure(figsize=(5, 3)))
            self.layout.addWidget(figure_canvas)

            if _type == "realtime_comparison":
                axes = RealtimeComparisonAxes(figure_canvas.figure.add_subplot(axes_class=axisartist.Axes), _property)
                self.figure_canvas_axes.append(axes)
            elif _type == "default":
                axes = figure_canvas.figure.add_subplot()
                self.figure_canvas_axes.append(axes)

    def update_figure_canvas(self, _index: int, _data: Tuple[float, float]):
        print("FigureCanvasUi --> figure_canvas_axes: ", self.figure_canvas_axes)
        # self.figure_canvas_axes[_index].update_data(_data)


class FigureCanvasUiFactory(QtWidgets.QWidget):
    factory: Dict[str, FigureCanvasUi] = {}
    get_figure_canvas_ui_sign = Qt.pyqtSignal(str, str, RealtimeAxesProperty)
    update_figure_canvas_sign = Qt.pyqtSignal(str, int, tuple)

    def __init__(self):
        super().__init__()
        self.get_figure_canvas_ui_sign.connect(self.get_figure_canvas_ui)
        self.update_figure_canvas_sign.connect(self.update_figure_canvas)

    @staticmethod
    def get_figure_canvas_ui(_window_name: str, _type: str, _axes_property: RealtimeAxesProperty):
        FigureCanvasUiFactory.factory[_window_name] = FigureCanvasUi(_window_name)
        FigureCanvasUiFactory.factory[_window_name].add_figure_canvas(_type, _axes_property)
        FigureCanvasUiFactory.factory[_window_name].show()

    @staticmethod
    def update_figure_canvas(_window_name: str, _index: int, _data: Tuple[float, float]):
        FigureCanvasUiFactory.factory[_window_name].update_figure_canvas(_index, _data)


if __name__ == '__main__':
    q_app = QtWidgets.QApplication(sys.argv)

    figure_canvas_ui_factory = FigureCanvasUiFactory()

    def get_test():
        axes_property = RealtimeComparisonAxesProperty("test-axes")
        figure_canvas_ui_factory.get_figure_canvas_ui_sign.emit("test", "realtime_comparison", axes_property)
        figure_canvas_ui_factory.get_figure_canvas_ui_sign.emit("test0-default", "default", RealtimeAxesProperty())
        time.sleep(1)

        print(FigureCanvasUiFactory.factory)
        while True:
            time.sleep(0.3)
            print("update_figure_canvas")
            figure_canvas_ui_factory.update_figure_canvas_sign.emit("test", 0,
                                                                    (random.uniform(-30, 30),
                                                                     random.uniform(-30, 30)))
            # figure_canvas_ui_factory.update_figure_canvas_sign.emit("test0-default", 0,
            #                                                         (random.uniform(-30, 30),
            #                                                          random.uniform(-30, 30)))
            # FigureCanvasUiFactory.factory["test"].update_figure_canvas(0, (random.uniform(-30, 30),
            #                                                                random.uniform(-30, 30)))

    threading.Thread(target=get_test).start()

    q_app.exec_()
