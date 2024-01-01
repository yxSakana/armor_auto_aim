set(NEED_HIK_UI ON) # 是否需要Hik_Ui

add_library(hik_driver STATIC include/HikDriver/HikDriver.h src/HikDriver.cpp)
set_target_properties(hik_driver PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(hik_driver PUBLIC include)
target_include_directories(hik_driver PUBLIC /opt/MVS/include)

find_package(Threads)
find_package(fmt REQUIRED)
find_package(glog REQUIRED)
find_package(OpenCV REQUIRED)
target_link_libraries(hik_driver PUBLIC
    Threads::Threads
    glog::glog
    fmt::fmt
    ${OpenCV_LIBS}
    X11
    MvCameraControl)

############
#  Hik Ui  #
############
if (NEED_HIK_UI STREQUAL "ON")
    add_library(hik_ui STATIC include/HikDriver/HikUi.h)
    set_target_properties(hik_ui PROPERTIES
        LINKER_LANGUAGE CXX
        )
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    target_link_libraries(hik_ui PUBLIC
        hik_driver
        vision_debug_widgets

        Qt5::Core
        Qt5::Widgets)
endif ()
