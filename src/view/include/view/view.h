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

        m_map["x"] = new DynamicLineChartView(this);
        m_map["v_x"] = new DynamicLineChartView(this);
        m_layout->addWidget(m_map["x"]->getChartView(), 0, 0);
        m_layout->addWidget(m_map["v_x"]->getChartView(), 1, 0);

        m_widget = new QWidget;
        m_widget->setLayout(m_layout);
        this->setCentralWidget(m_widget);
    }

    DynamicLineChartView* get(const std::string& key) const {
        const QString& k = QString::fromStdString(key);
        if (m_map.contains(k)) {
            return m_map[k];
        } else {
            return nullptr;
        }
    }

    bool insert(const std::string& key, const double& x, const double& y) {
        const QString& k = QString::fromStdString(key);
        if (m_map.contains(k)) {
            m_map[k]->insert(QPointF(x, y));
            return true;
        } else {
            return false;
        }
    }

    QPointF getLastPoint(const std::string& key) {
        const QString& k = QString::fromStdString(key);
        if (m_map.contains(k)) {
            return m_map[k]->getLastPoint();
        } else {
            return {};
        }
    }
private:
    using ChartViewMap = QMap<QString, DynamicLineChartView*>;
    ChartViewMap m_map;
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
