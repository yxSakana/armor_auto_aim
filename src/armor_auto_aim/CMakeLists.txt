set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_VERSION 5)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(armor_auto_aim STATIC ${sources})
set_target_properties(armor_auto_aim PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(armor_auto_aim PUBLIC include)

target_link_libraries(armor_auto_aim PUBLIC
    view
    hik_driver
    hik_ui
    armor_detector
    armor_tracker
    solver
    safe_container
    thread_pool
    serial_port
    google_logger
    debug_toolkit)

set(QT_PACKAGES
    Core)
foreach (QT_PACKAGE ${QT_PACKAGES})
    find_package(Qt${QT_VERSION} COMPONENTS ${QT_PACKAGE} REQUIRED)
endforeach ()
foreach (QT_PACKAGE ${QT_PACKAGES})
    target_link_libraries(view Qt${QT_VERSION}::${QT_PACKAGE})
endforeach ()
