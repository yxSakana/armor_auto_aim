# -*- coding: utf-8 -*-
"""  
  @projectName socket
  @file log_configure.py
  @brief 
 
  @author yx 
  @date 2023-11-04 18:27
"""

import sys

from loguru import logger

is_logger_initiated = False


def init_logger_configure():
    global is_logger_initiated
    if not is_logger_initiated:
        logger.remove()
        logger.add(sys.stderr, colorize=True, format=custom_format)
        is_logger_initiated = True


def custom_format(record):
    def with_prefix():
        return f"({record['time']:YYYY-MM-DD HH:mm:ss})[{record['level']}]({record['module']}): {record['message']}\n"
    if record["level"].name == "DEBUG":
        return f"<italic>{with_prefix()}</italic>"
    if record["level"].name == "INFO":
        return f"({record['time']:YYYY-MM-DD HH:mm:ss})<blue>[{record['level']}]</blue><magenta>({record['module']})</magenta>: <green>{record['message']}</green>\n"
    if record["level"].name == "WARNING":
        return f"<yellow>{with_prefix()}</yellow>"
    if record["level"].name == "ERROR":
        return f"<red>{with_prefix()}</red>"
    if record["level"].name == "CRITICAL":
        return f"<italic><bold><red>{with_prefix()}</red></bold></italic>"
    else:
        return with_prefix()
