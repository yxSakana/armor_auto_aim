# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file realtime_comparison.py
  @brief 
 
  @author yx 
  @date 2023-11-05 15:06
"""

import random
import time
from typing import Tuple, List

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from mpl_toolkits import axisartist

from plot.realtime_interface import RealtimeAxesInterface


class RealtimeComparisonAxesProperty(object):
    def __init__(self, axes_title: str, data_name: str = "y", data_unit: str = "m"):
        self.axes_title = axes_title
        self.data_name = data_name
        self.data_unit = data_unit


class RealtimeComparisonAxes(RealtimeAxesInterface):
    def __init__(self, _axes: axisartist.Axes, comparison_axes_property: RealtimeComparisonAxesProperty):
        super().__init__(_axes, comparison_axes_property)

        self.t_data: List = []
        self.measurement_data: List = []
        self.prediction_data: List = []
        self.ShowMaxTimeThreshold: int = 10
        self.measurement_data_setter: plt.Line2D = self.axes.plot(
            self.t_data, self.measurement_data, color="green", label="measurement")[0]
        self.prediction_data_setter: plt.Line2D = self.axes.plot(
            self.t_data, self.prediction_data, color="orange", label="prediction", linestyle="--")[0]
        self.init_canvas()

    def init_canvas(self):
        self.axes.axis["y=0"] = self.axes.new_floating_axis(nth_coord=0,
                                                            value=0,
                                                            axis_direction="bottom")
        self.axes.axis["y=0"].toggle(all=True)
        self.axes.axis["bottom", "top", "right"].set_visible(False)
        self.axes.set_ylabel(f"{self.axes_property.data_name}/{self.axes_property.data_unit}", rotation=45)
        self.axes.set_xlabel(f"t/s")
        self.axes.set_title(self.axes_property.axes_title)
        self.axes.legend()

    def update_data(self, data: Tuple[float, float]):
        self.measurement_data_setter.remove()
        self.prediction_data_setter.remove()
        self.measurement_data.append(data[0])
        self.prediction_data.append(data[1])
        t = self.t_data[-1] + 1 if self.t_data else 0
        self.t_data.append(t)

        for item in [self.t_data, self.measurement_data, self.prediction_data]:
            if len(item) > self.ShowMaxTimeThreshold:
                item.pop(0)

        self.measurement_data_setter.set_data(self.t_data, self.measurement_data)
        self.prediction_data_setter.set_data(self.t_data, self.prediction_data)
        self.prediction_data_setter.figure.canvas.draw()
        # self.measurement_data_setter: plt.Line2D = self.axes.plot(
        #     self.t_data, self.measurement_data, color="green", label="measurement")[0]
        # self.prediction_data_setter: plt.Line2D = self.axes.plot(
        #     self.t_data, self.prediction_data, color="orange", label="prediction", linestyle="--")[0]
        if len(self.t_data) != 1:
            self.axes.set_xlim(left=self.t_data[0], right=self.t_data[-1])

    @staticmethod
    def create_axes(_figure: plt.Figure, _grid_spec: gridspec.GridSpec, _row: int, _col: int):
        return _figure.add_subplot(_grid_spec[_row, _col], axes_class=axisartist.Axes)


if __name__ == "__main__":
    import matplotlib
    from matplotlib.animation import FuncAnimation

    from plot.realtime_factory import RealtimeFactory

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

        time.sleep(0.2)


    a = FuncAnimation(figure, func, interval=100, cache_frame_data=False)
    plt.show()
