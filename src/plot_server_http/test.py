# -*- coding: utf-8 -*-
"""  
  @projectName plot_server_http
  @file test.py
  @brief 
 
  @author yx 
  @date 2023-11-10 20:47
"""

import random
import time

import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation

matplotlib.use('TkAgg')

# 初始化物体的三维位置坐标
x, y, z = 1, 2, 3

# 创建 3D 图形对象
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 创建一个空的散点图，稍后通过 update 函数更新数据
scatter = ax.scatter([], [], [], c='r', marker='o')

# 设置坐标轴标签
ax.set_xlabel('X Label')
ax.set_ylabel('Y Label')
ax.set_zlabel('Z Label')


# 更新函数，用于更新散点图的位置
def update(frame):
    # 在这里更新物体的三维坐标
    # 这里简单地使用 frame 参数作为时间步来模拟动态效果
    ax.clear()
    new_x, new_y, new_z = random.uniform(-30, 30), random.uniform(-30, 30), random.uniform(-30, 30)
    print(new_x, new_y, new_z)
    ax.scatter(0, 0, 0, c='r', marker='o')
    ax.scatter(new_x, new_y, new_z, c='r', marker='o')
    time.sleep(0.5)
    scatter.set_offsets([[new_x, new_y, new_z]])


# 创建动画
animation = FuncAnimation(fig, update, frames=range(100), interval=100)

# 显示图形
plt.show()
