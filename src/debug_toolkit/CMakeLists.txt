file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(debug_toolkit ${sources})
set_target_properties(debug_toolkit PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(debug_toolkit PUBLIC include)

find_package(OpenCV REQUIRED)
target_link_libraries(debug_toolkit PUBLIC
        armor_tracker

        ${OpenCV_LIBS}
        )
