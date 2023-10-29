/**
 * @projectName armor_auto_aiming
 * @file test_hik_dirver.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-28 16:17
 */

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <HikDriver/HikDriver.h>
#include <HikDriver/HikReadThread.h>

namespace {
TEST(test_hik_driver, read_thread) {
    HikDriver hik_driver(0);
    HikReadThread hik_read_thread(&hik_driver);
    if (hik_driver.isConnected()) {
        LOG(INFO) << "Hik Connected!";
        hik_driver.setExposureTime(40000);
        hik_driver.setGain(10);
        hik_driver.showParamInfo();
        hik_read_thread.start();
        spdlogger spdlogger(__FUNCTION__);
        spdlogger.info("asdasd");
        std::this_thread::sleep_for(std::chrono::seconds(2));

        cv::Mat frame;
        while (true) {
            hik_read_thread.getRgbMat().copyTo(frame);
            cv::imshow("frame", frame);
            cv::waitKey(1);
        }
    }
}
}
