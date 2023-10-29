file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(spdlog_factory ${sources})
set_target_properties(spdlog_factory PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(spdlog_factory PUBLIC include)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(spdlog_factory PUBLIC
        spdlog
        fmt::fmt
        Qt5::Core
        )
