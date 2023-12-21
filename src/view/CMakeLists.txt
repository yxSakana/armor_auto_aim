set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_VERSION 5)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(view STATIC ${sources})
set_target_properties(view PROPERTIES
    LINKER_LANGUAGE CXX
    )
target_include_directories(view PUBLIC include)

target_link_libraries(view PUBLIC
    )

set(QT_PACKAGES
        Core Charts Widgets)
foreach (QT_PACKAGE ${QT_PACKAGES})
    find_package(Qt${QT_VERSION} COMPONENTS ${QT_PACKAGE} REQUIRED)
endforeach ()
foreach (QT_PACKAGE ${QT_PACKAGES})
    target_link_libraries(view Qt${QT_VERSION}::${QT_PACKAGE})
endforeach ()
