/**
 * @project_name test_qcharts
 * @file view.h
 * @brief
 * @author yx
 * @data 2023-12-21 17:08:08
 */

#ifndef TEST_QCHARTS_VIEW_H
#define TEST_QCHARTS_VIEW_H

#include <QMap>
#include <QMainWindow>
#include <QGridLayout>

#include <dynamic_chart_view/dynamic_line_chart_view.h>

namespace view {
class EkfView: public QMainWindow {
    Q_OBJECT
public:
    explicit EkfView(QWidget* parent = nullptr) : QMainWindow(parent) {
        m_layout = new QGridLayout;

        m_chart_views["x"] = new DynamicLineChartView(this);
        m_chart_views["x"]->getChart()->setTitle("Ekf X(Camera)");
        m_chart_views["x"]->addSeries("measure");
        m_chart_views["x"]->addSeries("predict");
        m_chart_views["x"]->addSeries("predict_shoot");
        m_chart_views["x"]->addSeries("measure_world");
        m_chart_views["x"]->addSeries("predict_world");
        m_layout->addWidget(m_chart_views["x"]->getChartView(), 0, 0);

        m_chart_views["v_x"] = new DynamicLineChartView(this);
        m_chart_views["v_x"]->getChart()->setTitle("Ekf Vx(World)");
        m_chart_views["v_x"]->addSeries("predict");
        m_layout->addWidget(m_chart_views["v_x"]->getChartView(), 1, 0);

        m_chart_views["y"] = new DynamicLineChartView(this);
        m_chart_views["y"]->getChart()->setTitle("Ekf Y(Camera)");
        m_chart_views["y"]->addSeries("measure");
        m_chart_views["y"]->addSeries("predict");
        m_chart_views["y"]->addSeries("measure_world");
        m_chart_views["y"]->addSeries("predict_world");
        m_layout->addWidget(m_chart_views["y"]->getChartView(), 0, 1);

        m_chart_views["v_y"] = new DynamicLineChartView(this);
        m_chart_views["v_y"]->getChart()->setTitle("Ekf Vy(World)");
        m_chart_views["v_y"]->addSeries("predict");
        m_layout->addWidget(m_chart_views["v_y"]->getChartView(), 1, 1);

        m_widget = new QWidget;
        m_widget->setLayout(m_layout);
        this->setCentralWidget(m_widget);
    }

    DynamicLineChartView* get(const std::string& key) const {
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
private:
    using ChartViewMap = QMap<QString, DynamicLineChartView*>;
    ChartViewMap m_chart_views;
    QWidget* m_widget;
    QGridLayout* m_layout;
};

class View: public QObject {
    Q_OBJECT
public:
    View() {
        m_ekf_view = new EkfView;
    }

    EkfView* m_ekf_view;
};
}

#endif //TEST_QCHARTS_VIEW_H
