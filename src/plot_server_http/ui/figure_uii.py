# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file figure_uii.py
  @brief 
 
  @author yx 
  @date 2023-11-09 18:58
"""
import pprint
import threading
from typing import Dict

from loguru import logger
import matplotlib
matplotlib.use('Qt5Agg')  # 或者 'Qt4Agg'，取决于你的Qt版本
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
    def __init__(self, figure_canvas: FigureCanvas, parent: QObject = None):
        super().__init__(parent)
        self._main_widget = QtWidgets.QWidget()
        self.setCentralWidget(self._main_widget)
        layout = QtWidgets.QVBoxLayout(self._main_widget)
        layout.addWidget(NavigationToolbar(figure_canvas, self))
        layout.addWidget(figure_canvas)
        logger.info(f"figure_canvas: {figure_canvas}")
        self.show()


class FigureCanvasUiFactory(QObject):
    __ui_factory: Dict = {}
    __factory_lock: threading.Lock = threading.Lock()
    create_ui_sign = pyqtSignal(str, dict)
    update_ui_sign = pyqtSignal(FigureCanvas)

    def __init__(self):
        super().__init__(None)
        self.create_ui_sign.connect(self.create_ui)
        self.update_ui_sign.connect(self.update_canvas)

    @staticmethod
    def create_ui(object_name: str, figure_pool: Dict):
        pprint.pprint(figure_pool)
        rows = int(figure_pool[object_name]["rows"])
        cols = int(figure_pool[object_name]["cols"])
        multiple_axes = figure_pool[object_name]["multiple_axes"]
        figure_canvas = FigureCanvas(plt.Figure(figsize=(5, 3)))
        grid_spec: GridSpec = GridSpec(rows, cols)
        for row in range(rows):
            for col in range(cols):
                current = multiple_axes[f"{row}{col}"]
                current["axes"] = RealtimeFactory.create_axes(figure_canvas, grid_spec, row, col, current)
        with FigureCanvasUiFactory.__factory_lock:
            ui = FigureCanvasUi(figure_canvas)
            FigureCanvasUiFactory.__ui_factory[object_name] = ui
            # ui.show()

        figure_pool[object_name]["figure_canvas"] = figure_canvas
        figure_pool[object_name]["grid_spec"] = grid_spec

    @staticmethod
    def update_canvas(figure_canvas: FigureCanvas):
        logger.info("updated canvas!")
        figure_canvas.draw()

    @staticmethod
    def get_ui(object_name: str) -> FigureCanvasUi:
        with FigureCanvasUiFactory.__factory_lock:
            return FigureCanvasUiFactory.__ui_factory[object_name]
