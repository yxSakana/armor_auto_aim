/**
 * @project_name test_qcharts
 * @file dynamic_lin_chart.cpp
 * @brief
 * @author yx
 * @data 2023-12-21 16:23:44
 */

#include <dynamic_chart_view/dynamic_line_chart_view.h>

namespace view {
DynamicLineChartView::DynamicLineChartView(QWidget* parent)
        : QWidget(parent) {
    m_timer = new QTimer(this);
    m_table = new DataTable;
    m_chart = new QtCharts::QChart;
    m_chart->createDefaultAxes();
    m_chart_view = new QtCharts::QChartView(m_chart);

    connect(m_timer, &QTimer::timeout, this, &DynamicLineChartView::updateViewSlot);
    m_timer->start(50);
}

void DynamicLineChartView::updateView() {
    // Show number
    for (auto it = m_table->begin(); it != m_table->end(); ++it) {
        int count = it.value()->count();
        if (count > m_show_number)
            it.value()->removePoints(0, count - m_show_number);
    }
    // x axes range
    double min_x, max_x;
    for (auto it = m_table->begin(); it != m_table->end(); ++it) {
        min_x = it.value()->at(0).x();
        max_x = it.value()->at(m_table->first()->count() - 1).x();
        m_min_x = std::min(min_x, m_min_x);
        m_max_x = std::max(max_x, m_max_x);
    }
    auto axes = m_chart->axes(Qt::Horizontal);
    if (!axes.empty())
        axes.first()->setRange(min_x, max_x + m_right_side);
    // y axes range
    double min_y, max_y;
    QVector<QPointF> points;
    for (auto it = m_table->begin(); it != m_table->end(); ++it) {
        points = it.value()->pointsVector();
        if (!points.empty()) {
            min_y = std::min_element(points.begin(), points.end(), [](const QPointF &p1, const QPointF &p2) {
                return p1.y() < p2.y();
            })->y();
            max_y = std::max_element(points.begin(), points.end(), [](const QPointF &p1, const QPointF &p2) {
                return p1.y() < p2.y();
            })->y();
            m_min_y = std::min(min_y, m_min_y);
            m_max_y = std::max(max_y, m_max_y);
        }
    }
    axes = m_chart->axes(Qt::Vertical);
    if (!axes.empty())
        axes.first()->setRange(m_min_y, m_max_y);

    m_chart_view->update();
}
}
