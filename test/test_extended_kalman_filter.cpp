/**
 * @projectName armor_auto_aiming
 * @file test_extended_kalmen_filter.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-21 10:12
 */

#include <test_utils/test_camera_start.h>
#include <extended_kalman_filter/extended_kalman_filter.h>

namespace {
class TestKf : public TestCamera {
protected:
    void SetUp() override {
        TestCamera::SetUp();
        //  xa  vxa ya vya za vza
        Q.setIdentity();
        R.setIdentity();
        Eigen::DiagonalMatrix<double, 6> p0;
        p0.setIdentity();
        //   x  vx  y  vy  z  yz
        F << 1, dt, 0,  0,  0, 0,  // x
             0, 1,  0,  0,  0, 0,  // vx
             0, 0,  1,  dt, 0, 0,  // y
             0, 0,  0,  1,  0, 0,  // vy
             0, 0,  0,  0,  0, dt, // z
             0, 0,  0,  0,  0,  1; // vz
        //     x  vx  y  vy  z  yz
        H <<   1, 0,  0, 0,  0, 0, // x
               0, 0,  1, 0,  0, 0, // y
               0, 0,  0, 0,  0, 1; // z
        auto f = [this](const Eigen::MatrixXd& x)->Eigen::MatrixXd {
            return ekf->getP() * x;
        };
        auto h = [this](const Eigen::MatrixXd& x)->Eigen::MatrixXd {
            return ekf->getH() * x;
        };
        auto j_f = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
            return F;
        };
        auto j_h = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
            return H;
        };
        auto update_Q = [this]()->Eigen::MatrixXd {
            return Q;
        };
        auto update_R = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
            return R;
        };

        ekf = std::make_unique<armor_auto_aiming::ExtendedKalmanFilter>(p0, f, h, j_f, j_h, update_Q, update_R);
    }
public:
    std::unique_ptr<armor_auto_aiming::ExtendedKalmanFilter> ekf;
    double dt{};
    Eigen::Matrix<double, 6, 6> F;
    Eigen::Matrix<double, 3, 6> H;
    Eigen::DiagonalMatrix<double, 6> Q;
    Eigen::DiagonalMatrix<double, 3> R;
    Eigen::VectorXd X;
    Eigen::VectorXd Z;
private:
};

class TestEkf : public TestCamera {
protected:
    void SetUp() override {
        TestCamera::SetUp();
    }
public:
    std::unique_ptr<armor_auto_aiming::ExtendedKalmanFilter> ekf;
};

TEST_F(TestKf, line_system) {
    if (hik_driver->isConnected()) {
        bool is_first = true;

        while (true) {
            tick_meter.reset();
            tick_meter.start();
            // Main
//            hik_read_thread->getRgbMat().copyTo(frame);
            auto& hik_frame = hik_read_thread->getFrame();
            hik_frame.getRgbFrame().copyTo(frame);
            LOG(INFO) << "time stamp: " << hik_frame.getTimestamp();

            bool status = inference->inference(frame, &inference_armors);
            if (status) {
                for (int i = 0; i < inference_armors.size(); ++i) {
                    for (int j = 0; j < 4; j++)
                        cv::line(frame, inference_armors[0].armor_apex[j], inference_armors[0].armor_apex[(j + 1) % 4],
                                 cv::Scalar(0, 0, 255), 3);
                    // Create armors
                    armors.emplace_back(inference_armors[i]);
                    // Pnp
                    armor_auto_aiming::solver::SpatialLocation spatial_location{};
                    bool code = pnp_solver->obtain3dCoordinates(armors[i], spatial_location);
                    LOG_IF_EVERY_N(INFO, code, 10) << spatial_location;
                    // kf
                    Z << spatial_location.x, spatial_location.y, spatial_location.z;
                    ekf->update();
                    ekf->predict(Z);
                    armor_auto_aiming::debug_toolkit::drawYawPitch(frame, spatial_location.yaw, spatial_location.pitch);
                }
            }
            // Fps info
            tick_meter.stop();
            fps = 1.0 / tick_meter.getTimeSec();
            LOG_EVERY_N(INFO, 5) << "FPS: " << fps;
            cv::imshow("frame", frame);
            cv::waitKey(1);
        }
    }
}

TEST_F(TestEkf, update) {

}

TEST_F(TestEkf, predict) {

}
}

