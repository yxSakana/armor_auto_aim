/**
 * @projectName armor_auto_aim
 * @file coord_utils.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-22 16:33
 */

#include <solver/coord_utils.h>

namespace armor_auto_aim {
Eigen::Vector3d rotationMatrixToEulerAngles(const Eigen::Matrix3d& R) {
    double val = std::sqrt(R(0, 0) * R(0, 0) + R(1, 0) * R(1, 0));
    bool is_singular = val < 1e-6;  // 判断奇异性
    double x, y, z;
    if (!is_singular) {
        x = atan2(R(2, 1), R(2, 2));
        y = atan2(-R(2, 0), val);
        z = atan2(R(1, 0), R(0, 0));
    } else {
        x = atan2(-R(1, 2), R(1, 1));
        y = atan2(-R(2, 0), val);
        z = 0;
    }
    return {z, y, x};
}

Eigen::Vector3d rotationVectorToEulerAngles(const cv::Mat& rvec) {
    cv::Mat rmat_cv;
    cv::Rodrigues(rvec, rmat_cv);
    Eigen::Matrix3d rmat_eigen;
    cv::cv2eigen(rmat_cv, rmat_eigen);
    return rmat_eigen.eulerAngles(2, 1, 0);
//    return rotationMatrixToEulerAngles(rmat_eigen);
}

std::string to_string(const Eigen::MatrixXd& matrix) {
    std::string info("[");
    long rows = matrix.rows();
    for (int i = 0; i < rows; ++i) {
        long cols = matrix.cols();
        for (int j = 0; j < cols; ++j) {
            info += std::to_string(matrix(i, j));
            if (j != cols - 1)
                info += ", ";
        }
        if (i != rows - 1)
            info += "\n";
    }
    info += "]";
    return info;
}
}
