set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_VERSION 5)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(serial_sort STATIC ${sources})
set_target_properties(serial_sort PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(serial_sort PUBLIC include)

set(QT_PACKAGES
        Core
        SerialPort
        )

foreach (QT_PACKAGE ${QT_PACKAGES})
    find_package(Qt${QT_VERSION} COMPONENTS ${QT_PACKAGE} REQUIRED)
endforeach ()
foreach (QT_PACKAGE ${QT_PACKAGES})
    target_link_libraries(serial_sort Qt${QT_VERSION}::${QT_PACKAGE})
endforeach ()