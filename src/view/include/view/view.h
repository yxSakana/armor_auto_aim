/**
 * @project_name armor_auto_aim
 * @file view.h
 * @brief
 * @author yx
 * @date 2023-12-21 17:08:08
 */

#ifndef ARMOR_AUTO_AIM_VIEW_VIEW_H
#define ARMOR_AUTO_AIM_VIEW_VIEW_H

#include <QMap>
#include <QMainWindow>
#include <QGridLayout>

#include <view/dynamic_chart_view.h>

namespace view {
template <typename T>
class BaseView: public QMainWindow {
protected:
    using ChartView = DynamicChartView<T>;
    using ChartViewMap = QMap<QString, ChartView*>;
public:
    explicit BaseView(QWidget* parent = nullptr): QMainWindow(parent) {
        m_layout = new QGridLayout;
        m_widget = new QWidget(this);
        m_widget->setLayout(m_layout);
        this->setCentralWidget(m_widget);
    }

    /**
     * @brief 获取指定 DynamicChartView
     * @param key
     * @return
     */
    [[nodiscard]] ChartView* getView(const std::string& key) const {
        const QString& k = QString::fromStdString(key);
        if (m_chart_views.contains(k)) {
            return m_chart_views[k];
        } else {
            return nullptr;
        }
    }

    bool insert(const std::string& key, const std::string& name, const double& x, const double& y) {
        const QString& k = QString::fromStdString(key);
        if (m_chart_views.contains(k)) {
            m_chart_views[k]->insert(name, QPointF(x, y));
            return true;
        } else {
            return false;
        }
    }

    QPointF getLastPoint(const std::string& key, const std::string& name) {
        const QString& k = QString::fromStdString(key);
        if (m_chart_views.contains(k)) {
            return m_chart_views[k]->getLastPoint(name);
        } else {
            return {};
        }
    }
protected:
    ChartViewMap m_chart_views; // 所有 动态View 的Map
    QWidget* m_widget;
    QGridLayout* m_layout;
};

class EkfView: public BaseView<QtCharts::QLineSeries> {
    Q_OBJECT
public:
    explicit EkfView(QWidget* parent = nullptr) : BaseView(parent) {
        setWindowTitle("Ekf View");
        m_chart_views["x"] = new ChartView(this);
        m_chart_views["x"]->getChart()->setTitle("Ekf X(Camera)");
//        m_chart_views["x"]->addSeries("measure");
//        m_chart_views["x"]->addSeries("predict");
//        m_chart_views["x"]->addSeries("predict_shoot");
        m_chart_views["x"]->addSeries("measure_world");
        m_chart_views["x"]->addSeries("predict_world");
        m_layout->addWidget(m_chart_views["x"]->getChartView(), 0, 0);

        m_chart_views["v_x"] = new ChartView(this);
        m_chart_views["v_x"]->getChart()->setTitle("Ekf Vx(World)");
        m_chart_views["v_x"]->addSeries("predict");
        m_layout->addWidget(m_chart_views["v_x"]->getChartView(), 1, 0);

        m_chart_views["y"] = new ChartView(this);
        m_chart_views["y"]->getChart()->setTitle("Ekf Y(Camera)");
//        m_chart_views["y"]->addSeries("measure");
//        m_chart_views["y"]->addSeries("predict");
        m_chart_views["y"]->addSeries("measure_world");
        m_chart_views["y"]->addSeries("predict_world");
        m_layout->addWidget(m_chart_views["y"]->getChartView(), 0, 1);

        m_chart_views["v_y"] = new ChartView(this);
        m_chart_views["v_y"]->getChart()->setTitle("Ekf Vy(World)");
        m_chart_views["v_y"]->addSeries("predict");
        m_layout->addWidget(m_chart_views["v_y"]->getChartView(), 1, 1);
    }
};

class TimestampView: public BaseView<QtCharts::QLineSeries> {
    Q_OBJECT
public:
    explicit TimestampView(QWidget* parent = nullptr)
        : BaseView(parent) {
        // Window
        setWindowTitle("Timestamp View");
        // Chart View
        m_chart_views["Camera-IMU timestamp"] = new ChartView(this);
        m_chart_views["Camera-IMU timestamp"]->getChart()->setTitle("Camera-IMU timestamp");
        // Series
        m_chart_views["Camera-IMU timestamp"]->addSeries("timestamp");
        m_chart_views["Camera-IMU timestamp"]->addSeries("now-camera");
        m_chart_views["Camera-IMU timestamp"]->addSeries("now-imu");
        m_layout->addWidget(m_chart_views["Camera-IMU timestamp"]->getChartView(), 0, 0);
    }
};

class EulerView: public BaseView<QtCharts::QScatterSeries> {
    Q_OBJECT
public:
    explicit EulerView(QWidget* parent = nullptr)
            : BaseView<QtCharts::QScatterSeries>(parent) {
        setWindowTitle("Euler View");
        m_chart_views["Euler"] = new ChartView(this);
        m_chart_views["Euler"]->setXFixed(true);
        m_chart_views["Euler"]->setYFixed(true);
        m_chart_views["Euler"]->setXMin(-180);
        m_chart_views["Euler"]->setXMax(180);
        m_chart_views["Euler"]->setYMin(-180);
        m_chart_views["Euler"]->setYMax(180);
        m_chart_views["Euler"]->getChart()->setTitle("Euler(Yaw Pitch)");
        m_chart_views["Euler"]->addSeries("receive imu");
        m_chart_views["Euler"]->addSeries("aim");
        m_layout->addWidget(m_chart_views["Euler"]->getChartView(), 0, 0);
    }
};

class FaceAngleView: public BaseView<QtCharts::QLineSeries> {
    Q_OBJECT
public:
    explicit FaceAngleView(QWidget* parent = nullptr)
            : BaseView<QtCharts::QLineSeries>(parent) {
        setWindowTitle("Face Angle View");
        m_chart_views["yaw"] = new ChartView(this);
        m_chart_views["yaw"]->setYFixed(true);
        m_chart_views["yaw"]->setYMin(0);
        m_chart_views["yaw"]->setYMax(360);
        m_chart_views["yaw"]->getChart()->setTitle("yaw");
        m_chart_views["yaw"]->addSeries("pose");
        m_chart_views["yaw"]->addSeries("predict");
        m_layout->addWidget(m_chart_views["yaw"]->getChartView(), 0, 0);
    }
};

class View: public QObject {
    Q_OBJECT
public:
    View() {
        m_ekf_view = new EkfView;
        m_timestamp_view = new TimestampView;
        m_imu_euler = new EulerView;
        m_face_angle_view = new FaceAngleView;
    }

    EkfView* m_ekf_view;
    TimestampView* m_timestamp_view;
    EulerView* m_imu_euler;
    FaceAngleView* m_face_angle_view;
};
}

#endif //ARMOR_AUTO_AIM_VIEW_VIEW_H
