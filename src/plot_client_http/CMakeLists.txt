file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(plot_client_http ${sources})
set_target_properties(plot_client_http PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(plot_client_http PUBLIC include)

find_package(httplib REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(nlohmann_json REQUIRED)
target_link_libraries(plot_client_http PUBLIC
        google_logger
        armor_tracker
        serial_sort

        Eigen3::Eigen
        httplib::httplib
        nlohmann_json::nlohmann_json
        )
