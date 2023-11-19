# -*- coding: utf-8 -*-
"""  
  @project_name plot_server_http
  @file realtime_waveform.py
  @brief
  @author yx
  @data 2023-11-19 15:13:20
"""

from typing import Tuple, List

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from mpl_toolkits import axisartist
from loguru import logger

from custom_axes.realtime_interface import RealtimeAxesWaveformInterface, RealtimeWaveformAxesPropertyInterface


class RealtimeWaveformAxesProperty(RealtimeWaveformAxesPropertyInterface):
    def __init__(self, _axes_title: str, _y_lim: Tuple[float, float],
                 _data_name: str = "realtime-waveform", _data_unit: str = "m"):
        super().__init__(_axes_title, _data_name, _data_unit)
        self.y_lim = _y_lim


class RealtimeWaveformAxes(RealtimeAxesWaveformInterface):
    __type_key = "realtime_waveform"

    def __init__(self, _axes: axisartist.Axes, _axes_property: RealtimeWaveformAxesProperty):
        super().__init__(_axes, _axes_property)

        self.t_data: List = []
        self.y_data: List = []

        self.data_setter = self.axes.plot(self.t_data, self.y_data,
                                          color="green",
                                          label=self.axes_property.data_name)[0]
        self.init_canvas()

    def init_canvas(self) -> None:
        super().init_canvas()
        # self.axes.set_ylim(bottom=self.axes_property.y_lim[0], top=self.axes_property.y_lim[1])

    def update_data(self, data: Tuple[float]) -> None:
        data = data[0]
        super().update_data((0.0, data))
        # resize array size
        self.y_data.append(data)
        if len(self.y_data) > self.ShowMaxTimeThreshold:
            self.y_data = self.y_data[len(self.y_data) - self.ShowMaxTimeThreshold:]
        # set data
        self.data_setter.set_data(self.t_data, self.y_data)
        # set lim
        y_lim_min = min((data, self.axes_property.y_lim[0]))
        y_lim_max = max((data, self.axes_property.y_lim[1]))
        self.axes.set_ylim(bottom=y_lim_min, top=y_lim_max)

    @staticmethod
    def create_axes(_figure: plt.Figure, _grid_spec: gridspec.GridSpec, _row: int, _col: int):
        return _figure.add_subplot(_grid_spec[_row, _col], axes_class=axisartist.Axes)
