# -*- coding: utf-8 -*-
"""  
  @project_name plot_server_http
  @file figure_canvas_ui.py
  @brief
  @author yx
  @data 2023-11-17 21:22:29
"""

import pprint
import threading
from typing import Dict, List, Any

from loguru import logger
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qtagg import FigureCanvas
from matplotlib.backends.backend_qtagg import \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.backends.qt_compat import QtWidgets
from matplotlib.figure import Figure
from matplotlib.gridspec import GridSpec
from PyQt5 import Qt
from PyQt5.Qt import pyqtSignal
from PyQt5.Qt import QObject, QWidget, QLabel
from PyQt5.Qt import QImage, QPixmap

from custom_axes.realtime_factory import RealtimeFactory


class FigureCanvasUi(QtWidgets.QMainWindow):
    __type: List[str] = ["realtime_comparison", "realtime_position"]

    def __init__(self):
        super().__init__()
        self._main = QtWidgets.QWidget()
        self.setCentralWidget(self._main)
        self.layout = QtWidgets.QHBoxLayout(self._main)

        self.figure_canvas_index: List[int] = []

    def add_figure_canvas(self, _type: str, _property: Any):
        if _type in FigureCanvasUi.__type:
            figure_canvas = FigureCanvas(Figure(figsize=(5, 3)))
            self.layout.addWidget(figure_canvas)
            self.figure_canvas_index.append(figure_canvas)

            if _type == "realtime_comparison":

