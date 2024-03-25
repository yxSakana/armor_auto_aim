海康相机驱动

官方驱动下载: https://www.hikrobotics.com/cn/machinevision/service/download?module=0

官方文档在: /opt/MVS/doc (根据具体下载路径更改)

## Usage(SDK)

---
编译安装SDK 

需要注意的是下载完后直接使用会链接不到库, 需要执行

```shell
sudo cp /opt/MVS/lib/64/* /usr/lib/
```

虽然直接复制一份库有些粗暴, 但不影响使用

```cmake
target_include_directories(${PROJECT_NAME} PUBLIC /opt/MVS/include)
target_link_libraries(${PROJECT_NAME} 
  PUBLIC
    X11
    MvCameraControl
)
```

---
或

将`SDK`的 `include` `lib` 添加到`driver`目录

```shell
hik_driver/
  - hik_sdk/
    - include/
      - hik_sdk/
        - ***.h
        - ***.h
    - lib/
      - ***.so
      - ***.so
  - include/
    - hik_driver/
      - ***.h
  - src/
    - ...
  - CMakeLists.txt
```

```cmake
target_include_directories(${PROJECT_NAME} PUBLIC hik_sdk/include)
target_link_directories(${PROJECT_NAME} PUBLIC hik_sdk/lib)
```

## Usage(driver)

```c++
HikDriver hik_driver(0);
if (hik_driver.isConnected()) {
    hik_driver.setExposureTime(4000);
    hik_driver.setGain(15);
    hik_driver.showParamInfo();
    hik_driver.startReadThread();
}
HikFrame frame = hik_driver.getFrame();
cv::Mat img = frame.getRgbFrame()->clone();
cv::imshow("frame", img);
```
