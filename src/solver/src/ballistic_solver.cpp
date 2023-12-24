/**
 * @project_name auto_aim
 * @file ballistic_solver.cpp
 * @brief
 * @author yx
 * @date 2023-11-25 15:31:40
 */

#include <solver/ballistic_solver.h>

#include <cmath>

#include <google_logger/google_logger.h>

namespace armor_auto_aim::solver {
double ballisticSolver(const Eigen::Vector3d& translation_vector,
                       const double& bullet_speed,
                       const int& max_number_iteration,
                       const int& max_number_kutta_iteration,
                       const float& stop_error,
                       const double& k,
                       const double& g) {
//    constexpr double k = 0.01903;  // 25°C, 1atm, 小弹丸
//    constexpr double k = 0.000556; // 25°C, 1atm, 大弹丸
//    constexpr double k = 0.000530; // 25°C, 1atm, 发光大弹丸
    // FIXME:  NaN Failed 是否因为 vertical 有时候为负
    // TODO: 世界坐标系下进行迭代
    double vertical = translation_vector[1]; //
    double temp_vertical = vertical;
    double horizontal = sqrt(translation_vector.squaredNorm() - vertical * vertical);
    double pitch = atan2(vertical, horizontal);
    double pitch_new = pitch;
    for (int i = 0; i < max_number_iteration; ++i) {
        double x = 0.0;
        double y = 0.0;
        double p = tan(pitch_new);
        double v = bullet_speed;
        double u = v / sqrt(1 + pow(p, 2));
        double delta_x = horizontal / max_number_kutta_iteration;
        for (int j = 0; j < max_number_kutta_iteration; ++j) {
            double k1_u = -k * u * sqrt(1 + pow(p, 2));
            double k1_p = -g / pow(u, 2);
            double k1_u_sum = u + k1_u * (delta_x / 2);
            double k1_p_sum = p + k1_p * (delta_x / 2);

            double k2_u = -k * k1_u_sum * sqrt(1 + pow(k1_p_sum, 2));
            double k2_p = -g / pow(k1_u_sum, 2);
            double k2_u_sum = u + k2_u * (delta_x / 2);
            double k2_p_sum = p + k2_p * (delta_x / 2);

            double k3_u = -k * k2_u_sum * sqrt(1 + pow(k2_p_sum, 2));
            double k3_p = -g / pow(k2_u_sum, 2);
            double k3_u_sum = u + k3_u * (delta_x / 2);
            double k3_p_sum = p + k3_p * (delta_x / 2);

            double k4_u = -k * k3_u_sum * sqrt(1 + pow(k3_p_sum, 2));
            double k4_p = -g / pow(k3_u_sum, 2);

            u += (delta_x / 6) * (k1_u + 2 * k2_u + 2 * k3_u + k4_u);
            p += (delta_x / 6) * (k1_p + 2 * k2_p + 2 * k3_p + k4_p);

            x += delta_x;
            y += p * delta_x;
        }
        double error = vertical - y;
        if (abs(error) <= stop_error) {
            break;
        } else {
            temp_vertical += error;
            pitch_new = atan2(temp_vertical, horizontal);
        }
    }
    return pitch_new - pitch;
}

void ballisticSolver_0() {
    float g = 9.822254;
    struct Point {
        float x{};
        float y{};

        Point(const float& _x, const float& _y) : x(_x), y(_y) {}

        std::string to_string() const {
            return fmt::format("[Point: ({}, {})]", x, y);
        }
    };

    Point target_point(100, 1);
    float bullet_velocity = 25;
    Point temp_point = target_point;
    for (int i = 0; i < 100; ++i) {
        float angle = atan2(temp_point.y, temp_point.x);

        float time = temp_point.y / (bullet_velocity * cos(angle));
        Point real_point(temp_point.x, (bullet_velocity * sin(angle) * time - 0.5 * g * time * time));
        float delta_height = target_point.y - real_point.y;
        temp_point.y += delta_height;

        LOG(INFO) << fmt::format("第 {} 次迭代: angle: {}； temp_point: {}; delta_height: {}",
                                 i, angle, temp_point.to_string(), delta_height);
    }
}
}
