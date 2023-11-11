# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file plot_ui.py
  @brief 
 
  @author yx 
  @date 2023-11-09 18:58
"""

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
