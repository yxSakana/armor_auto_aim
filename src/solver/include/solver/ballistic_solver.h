/**
 * @project_name auto_aim
 * @file ballistic_solver.h
 * @brief
 * @author yx
 * @data 2023-11-25 15:29:07
 */

#ifndef AUTO_AIM_BALLISTIC_SOLVER_H
#define AUTO_AIM_BALLISTIC_SOLVER_H

#include <Eigen/Dense>

namespace armor_auto_aim::solver {
 /**
  * @brief 四阶龙格-库塔法弹道解算
  * @param translation_vector 水平、垂直、距离
  * @param bullet_speed 子弹速度
  * @param max_number_iteration
  * @param max_number_kutta_iteration
  * @param stop_error
  * @param k
  * @param g
  * @return (new_pitch - pitch)补偿后的pitch与原来pitch的差值
  */
double ballisticSolver(const Eigen::Vector3d& translation_vector,
                       const double& bullet_speed,
                       const int& max_number_iteration = 10,
                       const int& max_number_kutta_iteration = 40,
                       const float& stop_error = 0.001,
                       const double& k = 0.01903,
                       const double& g = 9.781);

void ballisticSolver_0();
}

#endif //AUTO_AIM_BALLISTIC_SOLVER_H
