# -*- coding: utf-8 -*-
"""  
  @project_name plot_server_http
  @file realtime_position.py
  @brief
  @author yx
  @data 2023-11-12 17:12:57
"""


import random
from typing import Tuple, List
from custom_axes.realtime_interface import RealtimeAxesInterface

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec


class RealtimePositionAxesProperty(object):
    def __init__(self, axes_title: str,
                 x_val_name: str = "x", x_val_unit: str = "m",
                 y_val_name: str = "y", y_val_unit: str = "m"):
        self.axes_title = axes_title
        self.x_val_name: str = x_val_name
        self.x_val_unit: str = x_val_unit
        self.y_val_name: str = y_val_name
        self.y_val_unit: str = y_val_unit


class RealtimePositionAxes(RealtimeAxesInterface):
    def __init__(self, _axes: plt.Axes, position_axes_property: RealtimePositionAxesProperty):
        super().__init__(_axes, position_axes_property)

        self.x: float = 0
        self.y: float = 0
        self.point_setter: plt.PathCollection = self.axes.scatter([], [])
        self.init_canvas()

    def init_canvas(self):
        # 在四象限坐标系中，坐标轴交叉点即原点是 (0, 0)
        self.axes.axhline(0, color='black', linewidth=0.5)
        self.axes.axvline(0, color='black', linewidth=0.5)

        # 设置坐标轴的位置
        self.axes.spines['left'].set_position('zero')
        self.axes.spines['right'].set_color('none')
        self.axes.spines['bottom'].set_position('zero')
        self.axes.spines['top'].set_color('none')
        self.axes.set_xlabel(f"{self.axes_property.x_val_name}/{self.axes_property.x_val_unit}")
        self.axes.set_ylabel(f"{self.axes_property.y_val_name}/{self.axes_property.y_val_unit}")
        self.axes.set_title(self.axes_property.axes_title)

    def update_data(self, data: Tuple[float, float]):
        self.point_setter.remove()
        self.point_setter = self.axes.scatter(data[0], data[1], color="green")


if __name__ == "__main__":
    import matplotlib
    from matplotlib.animation import FuncAnimation
    matplotlib.use("TKAgg")

    figure: plt.Figure = plt.figure()
    grid_spec: gridspec.GridSpec = gridspec.GridSpec(1, 1)
    axes = RealtimePositionAxes.create_axes(figure, grid_spec, 0, 0)
    axes_property = RealtimePositionAxesProperty("axes_title")
    realtime_position_axes = RealtimePositionAxes(axes, axes_property)

    def func(i):
        data = random.uniform(-30, 30), random.uniform(-30, 30)
        realtime_position_axes.update_data(data)

    a = FuncAnimation(figure, func, interval=100, cache_frame_data=False)
    plt.show()
