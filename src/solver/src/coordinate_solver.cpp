/**
 * @project_name auto_aim
 * @file coordinate_solver.cpp
 * @brief
 * @author yx
 * @data 2023-11-26 14:14:15
 */

#include <solver/coord_utils.h>
#include <solver/coordinate_solver.h>
#include <google_logger/google_logger.h>

namespace armor_auto_aim::coordinate_solver {
cv::Point2d reproject(const Eigen::Matrix3d& camera_intrinsic, const Eigen::Vector3d& xyz) {
    Eigen::Vector3d result = camera_intrinsic * xyz * (1.0f / xyz[2]);
    return {result[0], result[1]};
}

cv::Point2d reproject(const std::array<double, 9>& camera_intrinsic, const Eigen::Vector3d& xyz) {
    Eigen::Matrix3d camera_intrinsic_eigen;
    for (int i = 0; i < 9; ++i)
        camera_intrinsic_eigen(i / 3, i % 3) = camera_intrinsic[i];
    return reproject(camera_intrinsic_eigen, xyz);
}

Eigen::Vector3d cameraToWorld(const Eigen::Vector3d& o1c_point,
                              const Eigen::Matrix3d& imu_rmat,
                              const Eigen::Matrix4d& transform_c2i,
                              const Eigen::Vector3d& tvec_i2w) {
    Eigen::Vector4d homogeneous_tvec(o1c_point[0], o1c_point[1], o1c_point[2], 1); // 装甲板在相机 坐标齐次
    Eigen::Vector4d imu_homogeneous_tvec = transform_c2i * homogeneous_tvec; // 装甲板在imu 齐次坐标
    Eigen::Vector3d imu_tvec(imu_homogeneous_tvec[0], imu_homogeneous_tvec[1], imu_homogeneous_tvec[2]); // 装甲板在imu 非齐次坐标
    const Eigen::Vector3d& world = imu_rmat * imu_tvec + tvec_i2w;

    float yaw, pitch, distance;
    std::string yaw_pitch_info;
    armor_auto_aim::getYawPitchDis(o1c_point, yaw, pitch, distance);
    yaw_pitch_info += fmt::format("\n\tCamera: yaw: {}; pitch: {};", yaw, pitch);
    armor_auto_aim::getYawPitchDis(imu_tvec, yaw, pitch, distance);
    yaw_pitch_info += fmt::format("\n\tIMU: yaw: {}; pitch: {};", yaw, pitch);
    armor_auto_aim::getYawPitchDis(world, yaw, pitch, distance);
    yaw_pitch_info += fmt::format("\n\tWorld: yaw: {}; pitch: {};", yaw, pitch);
    LOG(INFO) << fmt::format("\n\tCamera Point: {}, {}, {};\n"
                             "\tIMU Poin: {}, {}, {};\n"
                             "\tWorld Point: {}, {}, {};",
                             o1c_point[0], o1c_point[1], o1c_point[2],
                             imu_tvec[0], imu_tvec[1], imu_tvec[2],
                             world[0], world[1], world[2]);
    LOG(INFO) << yaw_pitch_info;
    LOG(INFO) << fmt::format("\nimu_rmat: \n{}", armor_auto_aim::to_string(imu_rmat));
    return world; // 装甲板在imu参考惯性坐标系下的坐标
}
}
