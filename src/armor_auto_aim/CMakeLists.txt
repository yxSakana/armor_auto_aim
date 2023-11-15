file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(armor_auto_aim STATIC ${sources})
set_target_properties(armor_auto_aim PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(armor_auto_aim PUBLIC include)

target_link_libraries(armor_auto_aim PUBLIC
        hik_driver
        armor_detector
        armor_tracker
        solver
        thread_pool
        serial_sort
        google_logger
        spdlog_factory
        plot_client_http
        debug_toolkit
)
