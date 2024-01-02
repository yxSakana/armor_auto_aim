/**
 * @project_name auto_aim
 * @file solver.cpp
 * @brief
 * @author yx
 * @date 2023-12-12 20:01:11
 */

#include <solver/solver.h>

#include <yaml-cpp/yaml.h>
#include <fmt/ranges.h>

#include <solver/coordinate_solver.h>
#include <solver/ballistic_solver.h>
#include <google_logger/google_logger.h>

namespace armor_auto_aim {
/*Solver::Solver()
        : pnp_solver(std::make_shared<PnPSolver>(
                m_camera_params.intrinsic_matrix, m_camera_params.distortion)) {}

Solver::Solver(const std::string& filename) {
    loadConfig(filename);
    pnp_solver = std::make_shared<PnPSolver>(m_camera_params.intrinsic_matrix,
                                             m_camera_params.distortion);
}

void Solver::loadConfig(const std::string& filename) {
    YAML::Node config = YAML::LoadFile(filename);
    if (!config) {
        LOG(ERROR) << "Failed: error load configure file: " << filename;
    } else {
        config = config["solver"];
    }
    // camera config
    m_camera_params.intrinsic_matrix = config["camera_params"]["intrinsic_matrix"].as<std::array<double, 9>>();
    m_camera_params.distortion = config["camera_params"]["distortion"].as<std::vector<double>>();
    // coordinate config
    auto T_ic = config["coordinate"]["T_ic"].as<std::vector<double>>();
    m_T_ic = Eigen::Map<Eigen::Matrix4d>(T_ic.data(), 4, 4);
    LOG(INFO) << "m_T_ic: "<< m_T_ic;
    auto tvec_i2w = config["coordinate"]["tvec_i2w"].as<std::array<double, 3>>();
    LOG(INFO) << fmt::format("tvec_i2w(array): {}", tvec_i2w);
    m_tvec_i2w = Eigen::Map<Eigen::Vector3d>(tvec_i2w.data(), 3, 1);
    LOG(INFO) << "tvec_i2w: " << m_tvec_i2w;
    // ballistic config
    m_ballistic_speed = config["ballistic"]["speed"].as<double>();
}*/
}
