/**
 * @projectName armor_auto_aim
 * @file draw_package.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-28 18:30
 */

#ifndef ARMOR_AUTO_AIMING_DRAW_PACKAGE_H
#define ARMOR_AUTO_AIMING_DRAW_PACKAGE_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <armor_tracker/tracker.h>

namespace armor_auto_aim::debug_toolkit {
/**
 *
 * @param points
 * @param center
 */
void pointSequence(cv::Point2f points[4], const cv::Point2f& center);

//void draw

/**
 * @brief 将旋转矩阵在图像上绘制出来
 *
 * @param src
 * @param r_rect
 * @param color
 * @param thickness
 */
void drawRotatedRect(cv::Mat &src, const cv::RotatedRect &r_rect, const cv::Scalar& color=cv::Scalar(255,255, 255), int thickness=3);

/**
 * @brief 将旋转矩阵在图像上绘制出来(并且会绘制出点的顺序)
 *
 * @param src
 * @param r_rect
 * @param color
 * @param thickness
 */
void drawRotateRectWithText(cv::Mat &src, const cv::RotatedRect &r_rect, const cv::Scalar& color=cv::Scalar(255,255, 255), int thickness=3);

/**
 * @brief 将旋转矩阵在图像上绘制出来(会矫正各个点并绘制出点的顺序)
 *
 * @param src
 * @param r_rect
 * @param color
 * @param thickness
 */
void drawRotateRectWithOrderWithText(cv::Mat &src, const cv::RotatedRect &r_rect, const cv::Scalar& color=cv::Scalar(255,255, 255), int thickness=3);

/**
 * @brief 多个旋转矩阵在图像上绘制出来
 * @param src
 * @param r_rects
 * @param color
 * @param thickness
 */
void drawRotatedRects(cv::Mat &src, const std::vector<cv::RotatedRect>& r_rects, const cv::Scalar& color=cv::Scalar(255,255, 255), int thickness=3);

/**
 * @brief 将多个旋转矩阵在图像上绘制出来(并且会绘制出点的顺序)
 *
 * @param src
 * @param r_rects
 * @param color
 * @param thickness
 */
void drawRotateRectsWithText(cv::Mat &src, const std::vector<cv::RotatedRect>& r_rects, const cv::Scalar& color=cv::Scalar(255,255, 255), int thickness=3);

/**
 * @brief 将多个旋转矩阵在图像上绘制出来(会矫正各个点并绘制出点的顺序)
 *
 * @param src
 * @param r_rects
 * @param color
 * @param thickness
 */
void drawRotateRectsWithOrderWithText(cv::Mat &src, const std::vector<cv::RotatedRect>& r_rects, const cv::Scalar& color=cv::Scalar(255,255, 255), int thickness=3);

/**
 *
 * @param src
 * @param yaw
 * @param pitch
 */
void drawYawPitch(const cv::Mat& src, const float& yaw, const float& pitch);

/**
 * @brief 绘制一帧的调试信息
 *
 * @param src
 * @param fps
 * @param timestamp
 */
void drawFrameInfo(cv::Mat& src, const std::vector<Armor>& armors, const Tracker& tracker,
                   const double& fps, const int64_t& timestamp, const float& dt);
}

#endif //ARMOR_AUTO_AIMING_DRAW_PACKAGE_H
