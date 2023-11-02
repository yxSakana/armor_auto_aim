/**
 * @projectName armor_auto_aiming
 * @file draw_package.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-28 18:33
 */

#include <debug_toolkit/draw_package.h>

namespace armor_auto_aiming::debug_toolkit {
void pointSequence(cv::Point2f points[4], const cv::Point2f& center)
{
    std::sort(points, points + 4, [&center](const cv::Point2f& p1, const cv::Point2f& p2){
        double angle1 = atan2(p1.y - center.y, p1.x - center.x);
        double angle2 = atan2(p2.y - center.y, p2.x - center.x);
        return angle1 < angle2;
    });
}

void drawRotatedRect(cv::Mat &src, const cv::RotatedRect &r_rect, const cv::Scalar& color, int thickness)
{
    cv::Point2f points[4];
    r_rect.points(points);

    for (int j = 0; j < 4; j++)
        cv::line(src, points[j], points[(j + 1) % 4], color, thickness);
}

void drawRotateRectWithText(cv::Mat &src, const cv::RotatedRect &r_rect, const cv::Scalar& color, int thickness)
{
    cv::Point2f points[4];
    r_rect.points(points);
    for (int i = 0; i < 4; i++) {
        cv::line(src, points[i], points[(i + 1) % 4], color, thickness);
        cv::putText(src, std::to_string(i), points[i], cv::FONT_HERSHEY_SIMPLEX, 3, color, thickness);
    }

    cv::putText(src, std::to_string(int(r_rect.angle)), r_rect.center, cv::FONT_HERSHEY_SIMPLEX, 3, color, thickness);
    cv::putText(src, std::to_string(int(r_rect.size.width)), cv::Point(r_rect.center.x + 80, r_rect.center.y + 80), cv::FONT_HERSHEY_SIMPLEX, 3, color, thickness);
    cv::putText(src, std::to_string(int(r_rect.size.height)), cv::Point(r_rect.center.x + 80, r_rect.center.y + 160), cv::FONT_HERSHEY_SIMPLEX, 3, color, thickness);
}

void drawRotateRectWithOrderWithText(cv::Mat &src, const cv::RotatedRect &r_rect, const cv::Scalar& color, int thickness)
{
    cv::Point2f points[4];
    r_rect.points(points);

    pointSequence(points, r_rect.center);

    for (int i = 0; i < 4; i++) {
        cv::line(src, points[i], points[(i + 1) % 4], color, thickness);
        cv::putText(src, std::to_string(i), points[i], cv::FONT_HERSHEY_SIMPLEX, 3, color, thickness);
    }

    cv::putText(src, std::to_string(int(r_rect.angle)), r_rect.center, cv::FONT_HERSHEY_SIMPLEX, 3, color, thickness);
    cv::putText(src, std::to_string(int(r_rect.size.width)), cv::Point(r_rect.center.x + 80, r_rect.center.y + 80), cv::FONT_HERSHEY_SIMPLEX, 3, color, thickness);
    cv::putText(src, std::to_string(int(r_rect.size.height)), cv::Point(r_rect.center.x + 80, r_rect.center.y + 160), cv::FONT_HERSHEY_SIMPLEX, 3, color, thickness);
}

void drawRotatedRects(cv::Mat &src, const std::vector<cv::RotatedRect>& r_rects, const cv::Scalar& color, int thickness)
{
    for (const cv::RotatedRect& r_rect: r_rects)
        drawRotatedRect(src, r_rect, color, thickness);
}

void drawRotateRectsWithText(cv::Mat &src, const std::vector<cv::RotatedRect>& r_rects, const cv::Scalar& color, int thickness)
{
    for (const cv::RotatedRect& r_rect: r_rects)
        drawRotateRectWithText(src, r_rect, color, thickness);
}

void drawRotateRectsWithOrderWithText(cv::Mat &src, const std::vector<cv::RotatedRect>& r_rects, const cv::Scalar& color, int thickness)
{
    for (const cv::RotatedRect& r_rect: r_rects)
        drawRotateRectWithOrderWithText(src, r_rect, color, thickness);
}

void drawYawPitch(const cv::Mat& src, const float& yaw, const float& pitch) {
    auto width = static_cast<float>(src.cols);
    auto height = static_cast<float>(src.rows);
    cv::Mat canvas = cv::Mat::zeros(cv::Size(src.cols, src.rows), src.type());
    cv::Scalar point_color(0, 0, 255);
    cv::Point2f offer(width / 2, height / 2);

    float w_rate = yaw / 90.0f;
    float h_rate = pitch / 90.0f;
    cv::Point2f point(width * w_rate, height * h_rate);
    point += offer;
    cv::circle(canvas, point, 6, point_color, -1);
    cv::imshow("drawYawPitch", canvas);
}
}
