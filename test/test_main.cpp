/**
 * @projectName armor_auto_aim
 * @file test_main.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-19 21:06
 */

#include <gtest/gtest.h>
#include <google_logger/google_logger.h>

int main(int argc, char* argv[]) {
    armor_auto_aim::google_log::initGoogleLogger("armor_auto_aim");

    testing::InitGoogleTest(&argc, argv);
    auto code = RUN_ALL_TESTS();

    return code;
}
