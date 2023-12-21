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
    m_series = new QtCharts::QLineSeries;
    m_chart = new QtCharts::QChart;
    m_chart->addSeries(m_series);
    m_chart->createDefaultAxes();
    m_chart_view = new QtCharts::QChartView(m_chart);

    connect(m_timer, &QTimer::timeout, this, &DynamicLineChartView::updateViewSlot);
    m_timer->start(50);
}

void DynamicLineChartView::updateView() {
    // Show number
    int count = m_series->count();
    if (count > m_show_number)
        m_series->removePoints(0, count - m_show_number);
    // x axes range
    auto min_x = m_series->at(0).x();
    auto max_x = m_series->at(m_series->count() - 1).x();
    m_chart->axes(Qt::Horizontal).first()->setRange(min_x, max_x + m_right_side);
    // y axes range
    auto points = m_series->pointsVector();
    if (!points.empty()) {
        double min_y = std::min_element(points.begin(), points.end(), [](const QPointF &p1, const QPointF &p2) {
            return p1.y() < p2.y();
        })->y();
        double max_y = std::max_element(points.begin(), points.end(), [](const QPointF &p1, const QPointF &p2) {
            return p1.y() < p2.y();
        })->y();
        m_min_y = std::min(min_y, m_min_y);
        m_max_y = std::max(max_y, m_max_y);
        m_chart->axes(Qt::Vertical).first()->setRange(m_min_y, m_max_y);
    }

    m_chart_view->update();
}
}
