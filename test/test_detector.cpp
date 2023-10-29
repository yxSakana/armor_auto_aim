/**
 * @projectName armor_auto_aiming
 * @file test_detector.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-28 11:22
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

namespace {
TEST(test_detector, inferenct_parser) {
    HikDriver hik_driver(0);
    HikReadThread hik_read_thread(&hik_driver);

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
            bool status = inference.inference(frame, &inference_armors);
            if (status) {
                for (int i = 0; i < inference_armors.size(); ++i) {
                    for (int j = 0; j < 4; j++)
                        cv::line(frame, inference_armors[0].armor_apex[j], inference_armors[0].armor_apex[(j + 1) % 4], cv::Scalar(0, 0, 255), 3);
                }

                LOG(INFO) << "Detected!" << "size: " << inference_armors.size() << std::endl;
                LOG(INFO) << "inference_armors: " << inference_armors[0];
            }

            cv::imshow("frame", frame);
            cv::waitKey(1);
        }
    }
//    cv::VideoCapture video_capture("../video/video.avi");
//    // Run
//    if (video_capture.isOpened()) {
//        while (true) {
//            video_capture >> frame;
//            if (frame.empty())
//                break;
//            std::vector<armor_auto_aiming::InferenceResult> armors;
//            bool status = inference.inference(frame, &armors);
//            if (status) {
//                for (int j = 0; j < 4; j++)
//                    cv::line(frame, armors[0].points[j], armors[0].points[(j + 1) % 4], cv::Scalar(255, 255, 255), 2);
//
//                LOG(INFO) << "Detected!" << "size: " << armors.size() << std::endl;
//                LOG(INFO) << "armors: " << armors[0];
//            }
//            cv::imshow("frame", frame);
//            cv::waitKey(1000 / video_capture.get(cv::CAP_PROP_FPS));
//        }
//    }
}

TEST(test_parser, quicksort) {
    std::mt19937 rng;  // 使用 Mersenne Twister 引擎，也可以选择其他引擎
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);  // 生成 1 到 100 之间的整数

    std::vector<armor_auto_aiming::InferenceResult> v(10);
    for (int i = 0; i < 10; ++i) {
        v[i].probability = distribution(rng);
        LOG(INFO) << "概率: " << v[i].probability << ", ";
    }
    LOG(INFO) << "-------";
    armor_auto_aiming::inference_parser::quicksort(v, 0, v.size() - 1);

    for (int i = 0; i < 10; ++i) {
        LOG(INFO) << "概率: " << v[i].probability << ", ";
    }
}

TEST(test_parser, quadrilateralArea) {
    cv::Point2f p[4] = {
        cv::Point2f(2.0f, 3.0f),
        cv::Point2f(8.0f, 3.0f),
        cv::Point2f(10.0f, 7.0f),
        cv::Point2f(3.0f, 7.0f),
     };
    LOG(INFO) << "result: " << armor_auto_aiming::inference_parser::quadrilateralArea(p);
}
}
