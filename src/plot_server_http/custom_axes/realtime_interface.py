# -*- coding: utf-8 -*-
"""  
  @project_name plot_server_http
  @file realtime_interface.py
  @brief
  @author yx
  @date 2023-11-12 17:17:50
"""

from typing import Tuple, List, TypedDict, Any
from abc import ABC, abstractmethod

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from mpl_toolkits import axisartist


class RealtimeAxesProperty(ABC):
    pass


class RealtimeAxesInterface(ABC):
    def __init__(self, _axes: Any, axes_property: Any):
        self.axes = _axes
        self.axes_property = axes_property

    @abstractmethod
    def init_canvas(self) -> None:
        pass

    @abstractmethod
    def update_data(self, data: Tuple[float, float]) -> None:
        pass

    @staticmethod
    def create_axes(_figure: plt.Figure, _grid_spec: gridspec.GridSpec, _row: int, _col: int):
        return _figure.add_subplot(_grid_spec[_row, _col])


class RealtimeWaveformAxesPropertyInterface(RealtimeAxesProperty, ABC):
    def __init__(self, _axes_title: str,
                 _data_name: str = "realtime-waveform", _data_unit: str = "m"):
        super().__init__()
        self.axes_title = _axes_title
        self.data_name = _data_name
        self.data_unit = _data_unit


class RealtimeAxesWaveformInterface(RealtimeAxesInterface, ABC):
    def __init__(self, _axes: axisartist.Axes, axes_property: RealtimeWaveformAxesPropertyInterface):
        super().__init__(_axes, axes_property)

        self.t_data: List = []
        self.ShowMaxTimeThreshold: int = 40

    @abstractmethod
    def init_canvas(self) -> None:
        self.axes.axis["y=0"] = self.axes.new_floating_axis(nth_coord=0, value=0, axis_direction="bottom")
        self.axes.axis["y=0"].toggle(all=True)
        self.axes.axis["bottom", "top", "right"].set_visible(False)
        self.axes.set_ylabel(f"{self.axes_property.data_name}/{self.axes_property.data_unit}", rotation=45)
        self.axes.set_xlabel(f"t/s")
        self.axes.set_title(self.axes_property.axes_title)
        self.axes.legend()

    @abstractmethod
    def update_data(self, data: Tuple[float, float]) -> None:
        t = self.t_data[-1] + 1 if self.t_data else 0
        self.t_data.append(t)
        # resize array size
        if len(self.t_data) > self.ShowMaxTimeThreshold:
            self.t_data = self.t_data[len(self.t_data) - self.ShowMaxTimeThreshold:]
        # set lim
        if len(self.t_data) not in [0, 1]:
            self.axes.set_xlim(left=self.t_data[0], right=self.t_data[-1])

    @staticmethod
    def create_axes(_figure: plt.Figure, _grid_spec: gridspec.GridSpec, _row: int, _col: int):
        return _figure.add_subplot(_grid_spec[_row, _col], axes_class=axisartist.Axes)

