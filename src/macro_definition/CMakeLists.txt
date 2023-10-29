file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(macro_definition ${sources})
set_target_properties(macro_definition PROPERTIES
        LINKER_LANGUAGE CXX
        )
target_include_directories(macro_definition PUBLIC include)
