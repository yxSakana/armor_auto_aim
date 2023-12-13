/**
 * @project_name auto_aim
 * @file solver.h
 * @brief
 * @author yx
 * @data 2023-12-12 20:01:04
 */

#ifndef AUTO_AIM_SOLVER_H
#define AUTO_AIM_SOLVER_H

#include <array>

namespace armor_auto_aim {
struct CameraParams {
    std::array<double, 9> intrinsic_matrix;
    std::array<double, 5> distortion;
};

class Solver {
public:
    Solver() =default;
private:
    CameraParams m_camera_params;
    double g;
};
}
#endif //AUTO_AIM_SOLVER_H
