file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(armor_detector STATIC ${sources})
set_target_properties(armor_detector PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(armor_detector PUBLIC include)

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenVINO REQUIRED)
find_package(glog REQUIRED)
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
target_link_libraries(armor_detector PUBLIC
        solver

        Threads::Threads
        ${OpenCV_LIBS}
        openvino::runtime
        glog::glog
        fmt::fmt
        Eigen3::Eigen
        )
