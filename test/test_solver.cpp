/**
 * @projectName armor_auto_aim
 * @file test_solver.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-29 18:48
 */

#include <cmath>

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
#include <solver/ballistic_solver.h>

namespace {
TEST(test_pnp_solver, pnp) {
    HikDriver hik_driver(0);
    HikReadThread hik_read_thread(&hik_driver);
    // 帧率测试
    cv::TickMeter tick_meter;
    double fps{};

    std::array<double, 9> intrinsic_matrix {
         1664.5, -7.7583, 743.8596,
         0.0, 1669.6, 573.3738,
         0.0, 573.37, 1.0
    };
    std::vector<double> distortion_vector { -0.44044, 0.2949, 0, -0.0042, 0 };
    armor_auto_aim::PnPSolver pnp_solver(intrinsic_matrix, distortion_vector);

    armor_auto_aim::Inference inference;
    inference.initModel("../model/opt-0527-001.xml");

    cv::namedWindow("frame", cv::WINDOW_NORMAL);
    cv::Mat frame;

    if (hik_driver.isConnected()) {
        LOG(INFO) << "Hik Connected!";
        hik_driver.setExposureTime(40000);
        hik_driver.setGain(10);
        hik_driver.showParamInfo();
        hik_read_thread.start();
        std::this_thread::sleep_for(std::chrono::seconds(2));

        while (true) {
            tick_meter.reset();
            tick_meter.start();
            hik_read_thread.getRgbMat().copyTo(frame);
            std::vector<armor_auto_aim::InferenceResult> inference_armors;
            std::vector<armor_auto_aim::Armor> armors;
            bool status = inference.inference(frame, &inference_armors);
            if (status) {
                for (int i = 0; i < inference_armors.size(); ++i) {
                    for (int j = 0; j < 4; j++)
                        cv::line(frame, inference_armors[0].armor_apex[j], inference_armors[0].armor_apex[(j + 1) % 4],
                                 cv::Scalar(0, 0, 255), 3);
                    armors.emplace_back(inference_armors[i]);
                    armor_auto_aim::solver::Pose pose{};
                    bool code = pnp_solver.obtain3dPose(armors[i], pose);
                    LOG_IF_EVERY_N(INFO, code, 10) << pose;

                    armor_auto_aim::debug_toolkit::drawYawPitch(frame, pose.yaw, pose.pitch);
                }
            }

            tick_meter.stop();
            fps = 1.0 / tick_meter.getTimeSec();
            LOG_EVERY_N(INFO, 5) << "FPS: " << fps;

            cv::imshow("frame", frame);
            cv::waitKey(1);
        }
    }
}

TEST(test_solver, test_shortest_angular_distance) {
    auto angleToRadian = [](double angel) -> double {
        return (angel * M_PI) / 180;
    };
    auto radianToAngel = [](double radian) -> double {
        return (radian * 180) / M_PI;
    };

    EXPECT_EQ(radianToAngel(armor_auto_aim::shortestAngularDistance(angleToRadian(20.0), angleToRadian(520.0))),
              140.0);
}

TEST(test_solver, test_ballistic_solver) {
    Eigen::Vector3d translation_vector({0.5, 0.6, 8.0});
    double vertical = translation_vector(1);
    double horizontal = sqrt(translation_vector.squaredNorm() - vertical * vertical);
    double pitch = atan2(vertical, horizontal);
    LOG(INFO) << fmt::format("z: {}; horizontal: {}", translation_vector(2), horizontal);
    LOG(INFO) << fmt::format("my_pitch: {}; old_pitch: {}; new_pitch: {}; diff: {}",
                             atan2(translation_vector(1), translation_vector(2)) * 180 / M_PI,
                             pitch * 180 / M_PI,
                             armor_auto_aim::solver::ballisticSolver(translation_vector, 25) * 180 / M_PI,
                             (armor_auto_aim::solver::ballisticSolver(translation_vector, 25) - pitch) * 180 / M_PI);
}
}
