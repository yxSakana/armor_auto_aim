set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_VERSION 5)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(serial STATIC ${sources})
set_target_properties(serial PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(serial PUBLIC include)

set(QT_PACKAGES
        Core
        SerialPort
        )

foreach (QT_PACKAGE ${QT_PACKAGES})
    find_package(Qt${QT_VERSION} COMPONENTS ${QT_PACKAGE} REQUIRED)
endforeach ()
foreach (QT_PACKAGE ${QT_PACKAGES})
    target_link_libraries(serial Qt${QT_VERSION}::${QT_PACKAGE})
endforeach ()