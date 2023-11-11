/**
 * @projectName armor_auto_aim
 * @file test_camera_start.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-11-02 18:33
 */

#ifndef ARMOR_AUTO_AIMING_TEST_CAMERA_START_H
#define ARMOR_AUTO_AIMING_TEST_CAMERA_START_H

#include <random>

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

#include <google_logger/google_logger.h>
#include <HikDriver/HikDriver.h>
#include <HikDriver/HikReadThread.h>
#include <armor_detector/inference.h>
#include <armor_detector/parser.h>
#include <debug_toolkit/draw_package.h>
#include <solver/pnp_solver.h>

class TestCamera : public testing::Test {
protected:
    void SetUp() override {
        cv::namedWindow("frame", cv::WINDOW_NORMAL);

        hik_driver = std::make_unique<HikDriver>(0);
        hik_read_thread = std::make_unique<HikReadThread>(hik_driver.get());
//        hik_read_thread = new HikReadThread(hik_driver.get());
        pnp_solver = std::make_unique<armor_auto_aim::PnPSolver>(m_intrinsic_matrix, m_distortion_vector);
        inference = std::make_unique<armor_auto_aim::Inference>();
        inference->initModel("../model/opt-0527-001.xml");

        if (hik_driver->isConnected()) {
            LOG(INFO) << "Hik Connected!";
            hik_driver->setExposureTime(40000);
            hik_driver->setGain(10);
            hik_driver->showParamInfo();
            hik_read_thread->start();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } else {
            LOG(ERROR) << "Hik can't connected!";
        }
    }
public:
    std::unique_ptr<HikDriver> hik_driver;
    std::unique_ptr<HikReadThread> hik_read_thread;
//    HikReadThread* hik_read_thread;
    std::unique_ptr<armor_auto_aim::PnPSolver> pnp_solver;
    std::unique_ptr<armor_auto_aim::Inference> inference;
    std::vector<armor_auto_aim::InferenceResult> inference_armors;
    std::vector<armor_auto_aim::Armor> armors;

    cv::TickMeter tick_meter;
    cv::Mat frame;
    HikFrame hik_frame;
    double fps = 0;
private:
    std::array<double, 9> m_intrinsic_matrix {
            1664.5, -7.7583, 743.8596,
            0.0, 1669.6, 573.3738,
            0.0, 573.37, 1.0
    };
    std::vector<double> m_distortion_vector { -0.44044, 0.2949, 0, -0.0042, 0 };
};

#endif //ARMOR_AUTO_AIMING_TEST_CAMERA_START_H
