# -*- coding: utf-8 -*-
"""  
  @project_name plot_server_http
  @file realtime_factory.py
  @brief
  @author yx
  @data 2023-11-14 18:19:15
"""

from typing import Dict, Union

from loguru import logger
from matplotlib.backends.backend_qtagg import FigureCanvas

from custom_axes.realtime_interface import *
from custom_axes.realtime_comparison import *
from custom_axes.realtime_position import *
from custom_axes.realtime_waveform import *


class RealtimeFactory(object):
    __axes_types = ["realtime_comparison", "realtime_position", "realtime_waveform"]

    @staticmethod
    def create_axes(_figure_canvas: FigureCanvas, _grid_spec: plt.GridSpec, _row: int, _col: int,
                    _axes_property: Dict) -> Union[None,
                                                   RealtimeComparisonAxes,
                                                   RealtimePositionAxes,
                                                   RealtimeWaveformAxes]:
        if _axes_property["type"] not in RealtimeFactory.__axes_types:
            logger.error(f"Unknown type{_axes_property['type']}")
            return None

        if _axes_property["type"] == "realtime_comparison":
            axes_title = _axes_property["property"]["axes_title"]
            y_lim = _axes_property["property"]["y_lim"]
            data_name = _axes_property["property"]["data_name"]
            data_unit = _axes_property["property"]["data_unit"]
            axes_ = RealtimeComparisonAxes.create_axes(_figure_canvas.figure, _grid_spec, _row, _col)
            axes_property_ = RealtimeComparisonAxesProperty(axes_title, y_lim, data_name, data_unit)
            return RealtimeComparisonAxes(axes_, axes_property_)
        elif _axes_property["type"] == "realtime_position":
            axes_title = _axes_property["property"]["axes_title"]
            x_val_name = _axes_property["property"]["x_val_name"]
            x_val_unit = _axes_property["property"]["x_val_unit"]
            y_val_name = _axes_property["property"]["y_val_name"]
            y_val_unit = _axes_property["property"]["y_val_unit"]
            x_lim = _axes_property["property"]["x_lim"]
            y_lim = _axes_property["property"]["y_lim"]
            axes_ = RealtimePositionAxes.create_axes(_figure_canvas.figure, _grid_spec, _row, _col)
            axes_property_ = RealtimePositionAxesProperty(axes_title, x_lim, y_lim,
                                                          x_val_name, x_val_unit,
                                                          y_val_name, y_val_unit)
            return RealtimePositionAxes(axes_, axes_property_)

        elif _axes_property["type"] == "realtime_waveform":
            axes_title = _axes_property["property"]["axes_title"]
            data_name = _axes_property["property"]["data_name"]
            data_unit = _axes_property["property"]["data_unit"]
            y_lim = _axes_property["property"]["y_lim"]
            axes_ = RealtimeWaveformAxes.create_axes(_figure_canvas.figure, _grid_spec, _row, _col)
            axes_property_ = RealtimeWaveformAxesProperty(axes_title, y_lim, data_name, data_unit)
            return RealtimeWaveformAxes(axes_, axes_property_)
