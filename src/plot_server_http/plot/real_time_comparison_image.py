# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file real_time_comparison_image.py
  @brief 
 
  @author yx 
  @date 2023-11-05 15:06
"""

from typing import Tuple, List

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from mpl_toolkits import axisartist


class RealTimeComparisonImage(object):
    def __init__(self, figure: plt.Figure = None, grid_spec: gridspec.GridSpec = None,
                 row: int = 0, col: int = 0,
                 title: str = "real time comparison", val_name: str = "y", val_unit: str = "m",
                 val_label: Tuple[str] = ("measurement", "prediction")):
        self.figure: plt.Figure = plt.figure() if (figure is None) else figure
        self.grid_spec: gridspec.GridSpec = gridspec.GridSpec(1, 1) if (grid_spec is None) else grid_spec
        self.axes: axisartist.Axes = self.figure.add_subplot(self.grid_spec[row, col], axes_class=axisartist.Axes)

        self.title: str = f"{title} - {row}:{col}" if (title is not None) else "real time comparison"
        self.val_name: str = val_name if (val_name is not None) else "y"
        self.val_unit: str = val_unit if (val_unit is not None) else "m"
        self.val_label: Tuple[str] = val_label if (val_label is not None) else ("measurement", "prediction")

        self.t_data: List = []
        self.measurement_data: List = []
        self.prediction_data: List = []
        self.m_ShowMaxTimeThreshold: int = 30

    def push_back(self, data: Tuple):
        self.measurement_data.append(data[0])
        self.prediction_data.append(data[1])
        t = self.t_data[-1] + 1 if self.t_data else 0
        self.t_data.append(t)
        self.update_canvas()

    def update_canvas(self):
        self.axes.clear()
        for data in [self.t_data, self.measurement_data, self.prediction_data]:
            if len(data) > self.m_ShowMaxTimeThreshold:
                data.pop(0)

        self.axes.axis["y=0"] = self.axes.new_floating_axis(nth_coord=0,
                                                            value=0,
                                                            axis_direction="bottom")
        self.axes.axis["y=0"].toggle(all=True)
        self.axes.axis["bottom", "top", "right"].set_visible(False)
        self.axes.plot(self.t_data, self.measurement_data, color="green", label=self.val_label[0])
        self.axes.plot(self.t_data, self.prediction_data, color="orange", label=self.val_label[1], linestyle="--")
        self.axes.set_ylabel(f"{self.val_name}/{self.val_unit}", rotation=45)
        self.axes.set_xlabel(f"t/s")
        self.axes.set_title(self.title)
        self.axes.legend()
