file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(armor_tracker STATIC ${sources})
set_target_properties(armor_tracker PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(armor_tracker PUBLIC include)

find_package(Eigen3 REQUIRED)

target_link_libraries(armor_tracker PUBLIC
        armor_detector
        solver

        Eigen3::Eigen
        )
