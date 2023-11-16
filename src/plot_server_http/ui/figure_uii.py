# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file figure_uii.py
  @brief 
 
  @author yx 
  @date 2023-11-09 18:58
"""

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
from PyQt5.Qt import pyqtSignal
from PyQt5.Qt import QObject, QWidget, QLabel
from PyQt5.Qt import QImage, QPixmap


class FigureCanvasUi(QtWidgets.QMainWindow):
    def __init__(self, figure_canvas: FigureCanvas, parent: QObject = None):
        super().__init__(parent)
        self._main_widget = QtWidgets.QWidget()
        self.setCentralWidget(self._main_widget)
        layout = QtWidgets.QVBoxLayout(self._main_widget)
        # layout.addWidget(NavigationToolbar(figure_canvas, self))
        layout.addWidget(figure_canvas)


class FigureCanvasUiFactory(QObject):
    __ui_factory: Dict = {}
    __factory_lock: threading.Lock = threading.Lock()
    get_ui_sign = pyqtSignal(str)

    def __init__(self, parent: QObject = None):
        super().__init__(parent)
        self.get_ui_sign.connect(FigureCanvasUiFactory.create_ui)

    @staticmethod
    def create_ui(object_name: str):
        # if figure_canvas is None:
        #     logger.error("figure is None")
        #     return None

        dynamic_canvas = FigureCanvas(Figure(figsize=(5, 3)))
        FigureCanvasUiFactory.__factory_lock.acquire()
        ui = FigureCanvasUi(dynamic_canvas)
        FigureCanvasUiFactory.__ui_factory[object_name] = ui
        ui.show()
        FigureCanvasUiFactory.__factory_lock.release()
        print("End", FigureCanvasUiFactory.__ui_factory)

    @staticmethod
    def getUi(object_name: str) -> FigureCanvasUi:
        FigureCanvasUiFactory.__factory_lock.acquire()
        FigureCanvasUiFactory.__factory_lock.release()
        return FigureCanvasUiFactory.__ui_factory[object_name]
