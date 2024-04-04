/**
 * @project_name auto_aim
 * @file solver.h
 * @brief
 * @author yx
 * @date 2023-12-12 20:01:04
 */

#ifndef AUTO_AIM_SOLVER_H
#define AUTO_AIM_SOLVER_H

#include <array>

#include <fmt/ranges.h>

#include <solver/coordinate_solver.h>
#include <solver/ballistic_solver.h>
#include <google_logger/google_logger.h>
#include <solver/pnp_solver.h>

namespace armor_auto_aim {
struct SolverParams {
    double g;
    double ballistic_speed;
    std::array<double, 9> intrinsic_matrix;
    std::vector<double> distortion;
    Eigen::Matrix4d T_ic; // Camera -> IMu 变换矩阵
    Eigen::Matrix4d T_ci;
    Eigen::Vector3d tvec_i2w;
    Eigen::Vector3d tvec_w2i{};
};

class SolverBuilder {
public:
    SolverBuilder() =default;

    SolverBuilder& setG(const double& g) {
        m_params.g = g;
        return *this;
    }

    SolverBuilder& setBallSpeed(const double& speed) {
        m_params.ballistic_speed = speed;
        return *this;
    }

    SolverBuilder& setIntrinsicMatrix(const std::array<double, 9>& intrinsic) {
        m_params.intrinsic_matrix = intrinsic;
        return *this;
    }

    SolverBuilder& setDistortionCess(const std::vector<double>& distortion) {
        m_params.distortion = distortion;
        return *this;
    }

    SolverBuilder& setTic(const Eigen::Matrix4d& mat) {
        m_params.T_ic = mat;
        return *this;
    }

    SolverBuilder& setTci(const Eigen::Matrix4d& mat) {
        m_params.T_ci = mat;
        return *this;
    }

    SolverBuilder& setTvecI2C(const Eigen::Vector3d& tvec) {
        m_params.tvec_i2w = tvec;
        return *this;
    }

    SolverBuilder& setTvecC2I(const Eigen::Vector3d& tvec) {
        m_params.tvec_w2i = tvec;
        return *this;
    }

    SolverParams build() {
        return m_params;
    }
private:
    SolverParams m_params;
};

class Solver {
public:
    Solver() =default;

    explicit Solver(const SolverParams& solver_params) {
        m_params = solver_params;
        pnp_solver = std::make_shared<PnPSolver>(m_params.intrinsic_matrix,
                                                 m_params.distortion);
    }

    double ballisticSolver(const Eigen::Vector3d& translation_vector) const {
        return solver::ballisticSolver(translation_vector,
                                       m_params.ballistic_speed);
    }

    cv::Point2d reproject(const Eigen::Vector3d& xyz) const {
        return coordinate_solver::reproject(m_params.intrinsic_matrix, xyz);
    }

    Eigen::Vector3d cameraToImu(const Eigen::Vector3d& o1c_point) {
        return coordinate_solver::cameraToImu(o1c_point, m_params.T_ic);
    }

    Eigen::Vector3d imuToCamera(const Eigen::Vector3d& o1i_point) {
        return coordinate_solver::imuToCamera(o1i_point, m_params.T_ci);
    }

    /**
     * @param o1c_point 装甲板在相机坐标系下的坐标
     * @param imu_rmat imu四元数解算出的旋转矩阵
     * @return (imu参考的)世界坐标系下的坐标
     */
    Eigen::Vector3d cameraToWorld(const Eigen::Vector3d& o1c_point, const Eigen::Matrix3d& imu_rmat) const {
        return coordinate_solver::cameraToWorld(o1c_point, imu_rmat, m_params.T_ic, m_params.tvec_i2w);
    }

    Eigen::Vector3d worldToCamera(const Eigen::Vector3d& o1w_point, const Eigen::Matrix3d& imu_rmat) const {
        return coordinate_solver::worldToCamera(o1w_point, imu_rmat, m_params.T_ci, m_params.tvec_w2i);
    }

//    Eigen::Vector3d deviationCorrect(const Eigen::Vector3d& o1c_point);

    double getSpeed() const { return m_params.ballistic_speed; }

    void setParams(const SolverParams& params) { m_params = params; }

    std::shared_ptr<PnPSolver> pnp_solver;
private:
    SolverParams m_params;
};
}
#endif //AUTO_AIM_SOLVER_H
