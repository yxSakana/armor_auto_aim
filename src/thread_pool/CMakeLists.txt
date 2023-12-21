file(GLOB_RECURSE sources CONFIGURE_DEPENDS include/*.h)
add_library(thread_pool STATIC ${sources})
set_target_properties(thread_pool PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(thread_pool PUBLIC include)

find_package(Threads REQUIRED)
target_link_libraries(thread_pool PUBLIC
        Threads::Threads)
