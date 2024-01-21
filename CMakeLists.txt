cmake_minimum_required(VERSION 3.25)
project(auto_aim LANGUAGES CXX)

# C++ Version && C++ Include Directory Settings
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(IS_ENABLE_GTEST OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif ()

if (IS_ENABLE_GTEST STREQUAL "ON")
    enable_testing()
endif ()

# sub library
#add_subdirectory(src/macro_definition)
add_subdirectory(src/view)
add_subdirectory(src/plot_client_http)
add_subdirectory(src/debug_toolkit)
add_subdirectory(src/safe_container)
add_subdirectory(src/thread_pool)
add_subdirectory(src/google_logger)
add_subdirectory(src/spdlog_factory)
add_subdirectory(src/HikDriver)
add_subdirectory(src/serial_port)
add_subdirectory(src/armor_tracker)
add_subdirectory(src/solver)
add_subdirectory(src/armor_detector)
add_subdirectory(src/armor_auto_aim)
add_subdirectory(src/vision_debug_widgets)

if (IS_ENABLE_GTEST STREQUAL "ON")
    find_package(GTest REQUIRED)
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS test/*.cpp test/*.h)
else ()
    file(GLOB sources "src/main.cpp")
    message("-- Sources: ${sources}")
endif ()
add_executable(${PROJECT_NAME} ${sources})

target_link_libraries(${PROJECT_NAME} PUBLIC
    armor_auto_aim)
if (IS_ENABLE_GTEST STREQUAL "ON")
    target_include_directories(${PROJECT_NAME} PUBLIC
        test/test_utils/include)
    target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME})
endif ()

install(TARGETS auto_aim
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    RUNTIME DESTINATION .)
