/**
 * @project_name test_qcharts
 * @file dynamic_line_chart.h
 * @brief
 * @author yx
 * @date 2023-12-21 16:23:34
 */

#ifndef ARMOR_AUTO_AIMING_DYNAMIC_CHART_VIEW_H
#define ARMOR_AUTO_AIMING_DYNAMIC_CHART_VIEW_H

#include <QWidget>
#include <QTimer>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>

namespace view {
template <typename T>
class DynamicChartView: public QWidget {
    using Data = T;
    using DataTable = QMap<QString, Data*>;
public:
    explicit DynamicChartView(QWidget* parent = nullptr)
            : QWidget(parent) {
        m_timer = new QTimer(this);
        m_table = new DataTable;
        m_y_zero = new QtCharts::QLineSeries;
        m_chart = new QtCharts::QChart;
        m_y_zero->setName("Y Zero");
        m_chart->addSeries(m_y_zero);
        m_chart->createDefaultAxes();
        m_chart_view = new QtCharts::QChartView(m_chart);

        connect(m_timer, &QTimer::timeout, this, &DynamicChartView::updateViewSlot);
        m_timer->start(50);
    }

    void addSeries(const std::string& name) {
        QString k = QString::fromStdString(name);
        auto data = new Data;
        data->setName(k);
        m_chart->addSeries(data);
        (*m_table)[k] = data;
        m_chart->createDefaultAxes();
    }

    Data* get(const std::string& name) {
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

    void updateView() {
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
        auto ax = dynamic_cast<QtCharts::QValueAxis*>(axes.first());
        m_y_zero->setColor(QColor(0, 0, 0));
        m_y_zero->insert(0, {ax->min(), 0});
        m_y_zero->insert(1, {ax->max(), 0});
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
private:
    int m_show_number = 50;
    int m_right_side = 20;
    double m_min_x = 0.f;
    double m_max_x = 0.f;
    double m_min_y = 0.f;
    double m_max_y = 0.f;

    QTimer* m_timer;
    DataTable* m_table;
    QtCharts::QLineSeries* m_y_zero;
    QtCharts::QChart* m_chart;
    QtCharts::QChartView* m_chart_view;
private Q_SLOT:
    void updateViewSlot() { updateView(); };
};
}

#endif //ARMOR_AUTO_AIMING_DYNAMIC_CHART_VIEW_H
