set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS include/*.h)
add_library(vision_debug_widgets STATIC ${sources})
set_target_properties(vision_debug_widgets PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(vision_debug_widgets PUBLIC include)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(vision_debug_widgets PUBLIC
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        )


