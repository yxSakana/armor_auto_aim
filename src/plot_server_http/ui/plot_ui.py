# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file plot_ui.py
  @brief 
 
  @author yx 
  @date 2023-11-09 18:58
"""

from typing import Dict

from PyQt5.Qt import pyqtSignal
from PyQt5.Qt import QObject, QWidget, QLabel
from PyQt5.Qt import QImage, QPixmap


class PlotUi(QWidget):
    def __init__(self, parent: QObject = None):
        super().__init__(parent)

        self.plot_label = QLabel(self)
        self.showMaximized()

    def set_plot_image(self, img_path: str):
        image = QImage(img_path)
        pixmap = QPixmap(image)
        self.plot_label.setPixmap(pixmap)
        self.plot_label.setGeometry(100, 0, pixmap.width(), pixmap.height())


class PlotUiFactory(QObject):
    ui_factory: Dict = {}
    get_ui_sign = pyqtSignal(str)

    def __init__(self, parent: QObject = None):
        super().__init__(parent)
        self.get_ui_sign.connect(self.getUi)

    @staticmethod
    def getUi(object_name: str) -> PlotUi:
        if object_name not in PlotUiFactory.ui_factory.keys():
            ui = PlotUiFactory.ui_factory.get(object_name, PlotUi())
            PlotUiFactory.ui_factory[object_name] = ui
            print(PlotUiFactory.ui_factory)
            ui.show()
            return ui
        else:
            return PlotUiFactory.ui_factory[object_name]
