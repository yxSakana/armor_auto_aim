/**
 * @projectName armor_auto_aiming
 * @file test_solver.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-29 18:48
 */

#include <random>

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

#include <google_logger/google_logger.h>
#include <HikDriver/HikDriver.h>
#include <HikDriver/HikReadThread.h>
#include <detector/inference.h>
#include <detector/parser.h>
#include <debug_toolkit/draw_package.h>
#include <solver/pnp_solver.h>

namespace {
TEST(test_pnp_solver, pnp) {
    // TODO: gtest改进
    HikDriver hik_driver(0);
    HikReadThread hik_read_thread(&hik_driver);

    std::array<double, 9> intrinsic_matrix {
         1664.5, -7.7583, 743.8596,
         0.0, 1669.6, 573.3738,
         0.0, 573.37, 1.0
    };
    std::vector<double> distortion_vector { -0.44044, 0.2949, 0, -0.0042, 0 };
    armor_auto_aiming::PnPSolver pnp_solver(intrinsic_matrix, distortion_vector);

    armor_auto_aiming::Inference inference;
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
            hik_read_thread.getRgbMat().copyTo(frame);

            std::vector<armor_auto_aiming::InferenceResult> inference_armors;
            std::vector<armor_auto_aiming::Armor> armors;
            bool status = inference.inference(frame, &inference_armors);
            if (status) {
                for (int i = 0; i < inference_armors.size(); ++i) {
                    for (int j = 0; j < 4; j++)
                        cv::line(frame, inference_armors[0].armor_apex[j], inference_armors[0].armor_apex[(j + 1) % 4],
                                 cv::Scalar(0, 0, 255), 3);
                    armors.emplace_back(inference_armors[i]);
                    armor_auto_aiming::solver::SpatialLocation spatial_location{};
                    bool code = pnp_solver.obtain3dCoordinates(armors[i], spatial_location);
                    LOG_IF(INFO, code) << spatial_location;
                }

                LOG(INFO) << "Detected!" << "size: " << inference_armors.size() << std::endl;
                LOG(INFO) << "inference_armors: " << inference_armors[0];
            }

            cv::imshow("frame", frame);
            cv::waitKey(1);
        }
    }
}
}