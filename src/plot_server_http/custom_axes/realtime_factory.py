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


class RealtimeFactory(object):
    @staticmethod
    def create_axes(_figure_canvas: FigureCanvas, _grid_spec: plt.GridSpec, _row: int, _col: int,
                    _axes_property: Dict) -> Union[RealtimeComparisonAxes]:
        if _axes_property["type"] == "realtime_comparison":
            axes_title = _axes_property["property"]["axes_title"]
            data_name = _axes_property["property"]["data_name"]
            data_unit = _axes_property["property"]["data_unit"]
            axes_ = _figure_canvas.figure.add_subplot(_grid_spec[_row, _col], axes_class=axisartist.Axes)
            axes_property_ = RealtimeComparisonAxesProperty(axes_title, data_name, data_unit)
            return RealtimeComparisonAxes(axes_, axes_property_)
        else:
            logger.error(f"Unknown type{_axes_property['type']}")
