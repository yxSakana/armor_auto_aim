/**
 * @project_name test_qcharts
 * @file dynamic_line_chart.h
 * @brief
 * @author yx
 * @data 2023-12-21 16:23:34
 */

#ifndef TEST_QCHARTS_DYNAMIC_LINE_CHART_VIEW_H
#define TEST_QCHARTS_DYNAMIC_LINE_CHART_VIEW_H

#include <QWidget>
#include <QTimer>
#include <QChart>
#include <QChartView>
#include <QLineSeries>

namespace view {
class DynamicLineChartView: public QWidget {
Q_OBJECT
public:
    explicit DynamicLineChartView(QWidget* parent = nullptr);

    void addSeries(const std::string& name) {
        QString k = QString::fromStdString(name);
        auto data = new QtCharts::QLineSeries;
        data->setName(k);
        m_chart->addSeries(data);
        (*m_table)[k] = data;
        m_chart->createDefaultAxes();
    }

    QtCharts::QLineSeries* get(const std::string& name) {
        const QString&& k = QString::fromStdString(name);
        if (m_table->contains(k))
            return (*m_table)[k];
        else
            return nullptr;
    }

    void insert(const std::string& name, const float& x, const float& y) {
        const QString&& k = QString::fromStdString(name);
        if (m_table->contains(k))
            (*m_table)[k]->append(x, y);
    }
    void insert(const std::string& name, const QPointF& p) {
        const QString&& k = QString::fromStdString(name);
        if (m_table->contains(k))
            (*m_table)[k]->append(p);
    }

    QPointF getLastPoint(const std::string& name) const {
        const QString&& k = QString::fromStdString(name);
        if (m_table->contains(k))
            return (*m_table)[k]->at((*m_table)[k]->count() - 1);
        else
            return {};
    }

    void setShowNumber(const int& n) { m_show_number = n; };

    void setRightSide(const int& n) { m_right_side = n; };

    [[nodiscard]] QtCharts::QChartView* getChartView() const { return m_chart_view; }

    [[nodiscard]] QtCharts::QChart* getChart() const { return m_chart; }

    void updateView();
private:
    using Data = QtCharts::QLineSeries;
    using DataTable = QMap<QString, Data*>;

    int m_show_number = 50;
    int m_right_side = 20;
    double m_min_x = 0.f;
    double m_max_x = 0.f;
    double m_min_y = 0.f;
    double m_max_y = 0.f;

    QTimer* m_timer;
    DataTable* m_table;
    QtCharts::QChart* m_chart;
    QtCharts::QChartView* m_chart_view;
private slots:
    void updateViewSlot() { updateView(); };
};
}

#endif //TEST_QCHARTS_DYNAMIC_LINE_CHART_VIEW_H
