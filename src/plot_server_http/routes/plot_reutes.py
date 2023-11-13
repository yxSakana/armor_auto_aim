# -*- coding: utf-8 -*-
"""  
  @project_name plot_server_http
  @file plot_reutes.py
  @brief
  @author yx
  @data 2023-11-13 18:58:23
"""

from typing import Dict

from flask import Flask
from flask.views import MethodView


class PlotAPI(MethodView):
    FigurePool: Dict = {}
    def __init__(self):
        pass

    def create_window(self):

