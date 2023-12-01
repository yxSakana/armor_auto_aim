# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file realtime_comparison.py
  @brief 
 
  @author yx 
  @date 2023-11-05 15:06
"""

import math
from typing import Tuple, List, Union

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from mpl_toolkits import axisartist

from custom_axes.realtime_interface import RealtimeAxesInterface, RealtimeAxesProperty, RealtimeAxesWaveformInterface


class RealtimeComparisonAxesProperty(RealtimeAxesProperty):
    def __init__(self, axes_title: str,
                 y_lim: Tuple[float, float],
                 data_name: str = "y",
                 data_unit: str = "m",
                 ):
        self.axes_title = axes_title
        self.data_name = data_name
        self.data_unit = data_unit
        self.y_lim: Union[str, Tuple[float, float]] = y_lim


class RealtimeComparisonAxes(RealtimeAxesWaveformInterface):
    __type_key = "realtime_comparison"

    def __init__(self, _axes: axisartist.Axes, comparison_axes_property: RealtimeComparisonAxesProperty):
        super().__init__(_axes, comparison_axes_property)

        self.t_data: List = []
        self.measurement_data: List = []
        self.prediction_data: List = []

        self.ShowMaxTimeThreshold: int = 40
        self.auto_y_lim: bool = isinstance(self.axes_property.y_lim, str) and \
                                self.axes_property.y_lim == "auto"
        self.ylim_min = float("inf")
        self.ylim_max = float("-inf")

        self.measurement_data_setter: plt.Line2D = self.axes.plot(
            self.t_data, self.measurement_data, color="green", label="measurement")[0]
        self.prediction_data_setter: plt.Line2D = self.axes.plot(
            self.t_data, self.prediction_data, color="orange", label="prediction", linestyle="--")[0]
        self.init_canvas()

    def init_canvas(self):
        super().init_canvas()
        self.axes.set_ylim(bottom=self.axes_property.y_lim[0], top=self.axes_property.y_lim[1])

    def update_data(self, data: Tuple[float, float]):
        super().update_data(data)
        # append data
        self.measurement_data.append(data[0])
        self.prediction_data.append(data[1])
        # resize array size
        if len(self.measurement_data) > self.ShowMaxTimeThreshold:
            self.measurement_data = self.measurement_data[len(self.measurement_data) - self.ShowMaxTimeThreshold:]
        if len(self.prediction_data) > self.ShowMaxTimeThreshold:
            self.prediction_data = self.prediction_data[len(self.prediction_data) - self.ShowMaxTimeThreshold:]
        # setter set data
        self.measurement_data_setter.set_data(self.t_data, self.measurement_data)
        self.prediction_data_setter.set_data(self.t_data, self.prediction_data)
        # set lim
        if self.auto_y_lim:
            self.ylim_min = min((data[0], data[1], self.ylim_min))
            self.ylim_max = max((data[0], data[1], self.ylim_max))
            self.axes.set_ylim(bottom=self.ylim_min, top=self.ylim_max)

    @staticmethod
    def create_axes(_figure: plt.Figure, _grid_spec: gridspec.GridSpec, _row: int, _col: int):
        return _figure.add_subplot(_grid_spec[_row, _col], axes_class=axisartist.Axes)


if __name__ == "__main__":
    import random

    import matplotlib
    from matplotlib.animation import FuncAnimation

    from custom_axes.realtime_factory import RealtimeFactory

    matplotlib.use("TKAgg")

    axes_property = {
        'property': {
            'axes_title': 'track armor x',
            'data_name': 'xa',
            'data_unit': 'm'
        },
        'type': 'realtime_comparison'
    }
    figure: plt.Figure = plt.figure()
    grid_spec: gridspec.GridSpec = gridspec.GridSpec(1, 3)
    axes1 = RealtimeFactory.create_axes(figure, grid_spec, 0, 0, axes_property)
    axes2 = RealtimeFactory.create_axes(figure, grid_spec, 0, 1, axes_property)
    axes3 = RealtimeFactory.create_axes(figure, grid_spec, 0, 2, axes_property)


    def func(i):
        axes1.update_data((random.uniform(-30, 30), random.uniform(-30, 30)))
        axes2.update_data((random.uniform(-30, 30), random.uniform(-30, 30)))
        axes3.update_data((random.uniform(-30, 30), random.uniform(-30, 30)))

        # time.sleep(0.2)


    a = FuncAnimation(figure, func, interval=100, cache_frame_data=False)
    plt.show()
