file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(google_logger STATIC ${sources})
set_target_properties(google_logger PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(google_logger PUBLIC include)

find_package(fmt REQUIRED)
find_package(glog REQUIRED)
find_package(gflags REQUIRED)

target_link_libraries(google_logger PUBLIC
        fmt::fmt
        gflags
        glog::glog
        )
