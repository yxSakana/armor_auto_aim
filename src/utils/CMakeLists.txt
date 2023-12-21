file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(utils STATIC ${sources})
set_target_properties(utils PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(utils PUBLIC include)

find_package(Eigen3 REQUIRED)

target_link_libraries(utils PUBLIC
        Eigen3::Eigen
        )
