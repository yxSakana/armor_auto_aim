/**
 * @projectName armor_auto_aiming
 * @file test_main.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-19 21:06
 */

#include <gtest/gtest.h>
#include <google_logger/google_logger.h>

int main(int argc, char* argv[]) {
    armor_auto_aiming::google_log::initGoogleLogger("armor_auto_aiming");

    testing::InitGoogleTest(&argc, argv);
    auto code = RUN_ALL_TESTS();

    armor_auto_aiming::google_log::shutdownGoogleLogger();
    return code;
}
