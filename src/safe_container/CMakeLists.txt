file(GLOB_RECURSE sources CONFIGURE_DEPENDS include/*.h)
add_library(safe_container STATIC ${sources})
set_target_properties(safe_container PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(safe_container PUBLIC include)

target_link_libraries(safe_container PUBLIC
        )
