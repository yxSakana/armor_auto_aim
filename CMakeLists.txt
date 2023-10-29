cmake_minimum_required(VERSION 3.25)
project(armor_auto_aiming LANGUAGES CXX)

# C++ Version && C++ Include Directory Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(IS_ENABLE_GTEST ON)

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
add_subdirectory(src/debug_toolkit)
add_subdirectory(src/thread_pool)
add_subdirectory(src/google_logger)
add_subdirectory(src/spdlog_factory)
add_subdirectory(src/HikDriver)
add_subdirectory(src/serial)
add_subdirectory(src/extended_kalman_filter)
add_subdirectory(src/solver)
add_subdirectory(src/detector)

if (IS_ENABLE_GTEST STREQUAL "ON")
    find_package(GTest REQUIRED)
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS test/*.cpp test/*.h)
elseif ()
    file(sources src/main.cpp)
endif ()
add_executable(${PROJECT_NAME} ${sources})

target_link_libraries(${PROJECT_NAME} PUBLIC
#        macro_definition
        debug_toolkit
        thread_pool
        google_logger
        spdlog_factory
        hik_driver
        serial
        extended_kalman_filter
        solver
        detector
        )
if (IS_ENABLE_GTEST STREQUAL "ON")
    target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME})
endif ()
