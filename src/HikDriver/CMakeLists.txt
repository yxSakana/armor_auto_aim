set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(hik_driver STATIC ${sources})
set_target_properties(hik_driver PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(hik_driver PUBLIC include)
target_include_directories(hik_driver PUBLIC /opt/MVS/include)

find_package(fmt REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCV REQUIRED)
target_link_libraries(hik_driver PUBLIC
        spdlog_factory

        fmt::fmt
        ${OpenCV_LIBS}
        Qt5::Core
        Qt5::Widgets
        X11
        MvCameraControl
        )

