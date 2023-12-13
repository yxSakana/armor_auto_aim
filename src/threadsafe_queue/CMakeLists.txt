file(GLOB_RECURSE sources CONFIGURE_DEPENDS include/*.h)
add_library(threadsafe_queue STATIC ${sources})
set_target_properties(threadsafe_queue PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(threadsafe_queue PUBLIC include)

target_link_libraries(threadsafe_queue PUBLIC
        )
