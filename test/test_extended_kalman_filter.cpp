/**
 * @projectName armor_auto_aim
 * @file test_extended_kalmen_filter.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-21 10:12
 */

#include <test_utils/test_camera_start.h>
#include <armor_tracker/extended_kalman_filter.h>
#include <armor_tracker/tracker.h>
#include <plot_client_http/ekf_plot.h>
#include <plot_client_http/pnp_view.h>
#include <thread_pool/thread_pool.h>

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
             0, 0,  0,  0,  1, dt, // z
             0, 0,  0,  0,  1,  1; // vz
        //     x  vx  y  vy  z  yz
        H <<   1, 0,  0, 0,  0, 0, // x
               0, 0,  1, 0,  0, 0, // y
               0, 0,  0, 0,  1, 0; // z
        auto f = [this](const Eigen::MatrixXd& x)->Eigen::MatrixXd {
            return tracker->ekf->getF() * x;
        };
        auto h = [this](const Eigen::MatrixXd& x)->Eigen::MatrixXd {
            return tracker->ekf->getH() * x;
        };
        auto j_f = [this](const Eigen::MatrixXd&)->Eigen::MatrixXd {
            //   x  vx  y  vy   z   yz
            F << 1, dt, 0,  0,  0,  0,  // x
                 0, 1,  0,  0,  0,  0,  // vx
                 0, 0,  1,  dt, 0,  0,  // y
                 0, 0,  0,  1,  0,  0,  // vy
                 0, 0,  0,  0,  1,  dt, // z
                 0, 0,  0,  0,  0,  1; // vz
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

        tracker = std::make_unique<armor_auto_aim::Tracker>();
        tracker->ekf = std::make_shared<armor_auto_aim::ExtendedKalmanFilter>(p0, f, h, j_f, j_h, update_Q, update_R);
        armor_auto_aim::ekf_plot::lineSystemCreateWindowRequest(&plot_client_http);
        armor_auto_aim::pnp_view::pnpViewCreateWindowRequest(&plot_client_http);
    }
public:
    armor_auto_aim::PlotClientHttp plot_client_http;
    std::unique_ptr<armor_auto_aim::Tracker> tracker;
    HikFrame hik_frame;
    armor_auto_aim::ThreadPool thread_pool;

    double dt{};
    int64_t last_t = std::chrono::system_clock::now().time_since_epoch().count();

    Eigen::Matrix<double, 6, 6> F;
    Eigen::Matrix<double, 3, 6> H;
    Eigen::DiagonalMatrix<double, 6> Q;
    Eigen::DiagonalMatrix<double, 3> R;
    Eigen::VectorXd X;
    Eigen::VectorXd Z;
};

class TestEkf : public TestCamera {
protected:
    void SetUp() override {
        TestCamera::SetUp();
    }
public:
};

TEST_F(TestKf, line_system) {
    if (hik_driver->isConnected()) {
        bool is_first = true;
        int plot_count = 0;

        while (true) {
            tick_meter.reset();
            tick_meter.start();
            // Main
//            hik_read_thread->getRgbMat().copyTo(frame);
//            int64_t timestamp = 0;
            hik_frame = hik_read_thread->getFrame();
            frame = hik_frame.getRgbFrame();
            int64_t timestamp = hik_frame.getTimestamp();
            inference_armors.clear();
            armors.clear();

            bool status = inference->inference(frame, &inference_armors);
            if (status) {
                for (int i = 0; i < inference_armors.size(); ++i) {
                    for (int j = 0; j < 4; j++)
                        cv::line(frame, inference_armors[0].armor_apex[j], inference_armors[0].armor_apex[(j + 1) % 4],
                                 cv::Scalar(0, 0, 255), 3);
                    // Create armors
                    armors.emplace_back(armor_auto_aim::InferenceResult(inference_armors[i]));
                    // Pnp
                    bool code = pnp_solver->obtain3dPose(armors[i]);
                    // debug draw
//                    armor_auto_aim::debug_toolkit::drawYawPitch(frame, armors[i].pose.yaw, armors[i].pose.pitch);
                }
            }
            // dt
            dt = static_cast<double>(timestamp - last_t);
            last_t = timestamp;
            // tracker
            if (tracker->state() == armor_auto_aim::TrackerStateMachine::State::Lost) {
                tracker->initTracker(armors);
            } else {
                tracker->updateTracker(armors);
            }
            // kf draw
//            plot_count++;
//            if (plot_count > 100) {
            thread_pool.enqueue(armor_auto_aim::ekf_plot::lineSystemUpdateDataRequest, &plot_client_http, *tracker);
            thread_pool.enqueue(armor_auto_aim::pnp_view::pnpViewUpdateDataRequest, &plot_client_http, *tracker);
//                plot_count = 0;
//            }
//            armor_auto_aim::ekf_plot::lineSystemUpdateDataRequest(&plot_client_http, *tracker);
            // Fps info
            tick_meter.stop();
            fps = 1.0 / tick_meter.getTimeSec();
            armor_auto_aim::debug_toolkit::drawFrameInfo(frame, armors, *tracker, fps, timestamp, dt);
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

