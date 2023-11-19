# -*- coding: utf-8 -*-
"""  
  @project_name plot_server_http
  @file realtime_interface.py
  @brief
  @author yx
  @data 2023-11-12 17:17:50
"""

from typing import Tuple, List, TypedDict, Any
from abc import ABC, abstractmethod

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec


class RealtimeAxesProperty(ABC):
    pass


class RealtimeAxesInterface(ABC):
    def __init__(self, _axes: Any, axes_property: Any):
        self.axes = _axes
        self.axes_property = axes_property

    @abstractmethod
    def init_canvas(self):
        pass

    @abstractmethod
    def update_data(self, data: Tuple[float, float]):
        pass

    @staticmethod
    def create_axes(_figure: plt.Figure, _grid_spec: gridspec.GridSpec, _row: int, _col: int):
        return _figure.add_subplot(_grid_spec[_row, _col])
