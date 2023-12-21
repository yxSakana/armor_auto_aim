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

    void insert(const float& x, const float& y) { m_series->append(x, y); }
    void insert(const QPointF& p) { m_series->append(p); }

    QPointF getLastPoint() const { return m_series->at(m_series->count() - 1); }

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
    double m_min_y = 0.f;
    double m_max_y = 0.f;

    QTimer* m_timer;
    QtCharts::QLineSeries* m_series;
    DataTable* m_table;
    QtCharts::QChart* m_chart;
    QtCharts::QChartView* m_chart_view;
private slots:
    void updateViewSlot() { updateView(); };
};
}

#endif //TEST_QCHARTS_DYNAMIC_LINE_CHART_VIEW_H
