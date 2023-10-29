file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(extended_kalman_filter STATIC ${sources})
set_target_properties(thread_pool PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(extended_kalman_filter PUBLIC include)

find_package(Eigen3 REQUIRED)

target_link_libraries(extended_kalman_filter PUBLIC
        Eigen3::Eigen
        )
