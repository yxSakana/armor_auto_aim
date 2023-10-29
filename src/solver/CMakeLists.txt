file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(solver STATIC ${sources})
set_target_properties(solver PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(solver PUBLIC include)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

target_link_libraries(solver PUBLIC
        ${OpenCV_LIBS}
        Eigen3::Eigen
        )
