/**
 * @project_name armor_auto_aim
 * @file dynamic_line_chart.h
 * @brief
 * @author yx
 * @date 2023-12-21 16:23:34
 */

#ifndef ARMOR_AUTO_AIMING_VIEW_DYNAMIC_CHART_VIEW_H
#define ARMOR_AUTO_AIMING_VIEW_DYNAMIC_CHART_VIEW_H

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
    explicit DynamicChartView(bool is_fix, QWidget* parent = nullptr)
            : m_is_fix(is_fix),
              QWidget(parent) {
        m_timer = new QTimer(this);
        m_table = new DataTable;
        m_y_zero = new QtCharts::QLineSeries;
        m_chart = new QtCharts::QChart;
        m_text = new QGraphicsSimpleTextItem(m_chart);

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

    /**
     * @brief 从容器中获取指定数据序列
     * @param name
     * @return
     */
    Data* getSeries(const std::string& name) {
        const QString&& k = QString::fromStdString(name);
        if (m_table->contains(k))
            return (*m_table)[k];
        else
            return nullptr;
    }

    double getXMin() const { return m_min_x; }
    void setXMin(double v) { m_min_x = v; }
    double getXMax() const { return m_max_x; }
    void setXMax(double v) { m_max_x = v; }
    double getYMin() const { return m_min_y; }
    void setYMin(double v) { m_min_y = v; }
    double getYMax() const { return m_max_y; }
    void setYMax(double v) { m_max_y = v; }

    void setShowNumber(const int& n) { m_show_number = n; };

    void setRightSide(const int& n) { m_right_side = n; };

    [[nodiscard]] QtCharts::QChartView* getChartView() const { return m_chart_view; }

    [[nodiscard]] QtCharts::QChart* getChart() const { return m_chart; }

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

    void replace(const std::string& name, const int& index, const float& x, const float& y) {
        const QString&& k = QString::fromStdString(name);
        if (m_table->contains(k))
            (*m_table)[k]->replace(index, QPointF(x, y));
    }

    QPointF getLastPoint(const std::string& name) const {
        const QString&& k = QString::fromStdString(name);
        if (m_table->contains(k))
            return (*m_table)[k]->at((*m_table)[k]->count() - 1);
        else
            return {};
    }

    void updateView() {
        // 减小数据.
        // Show number
        for (auto it = m_table->begin(); it != m_table->end(); ++it) {
            int count = it.value()->count();
            if (count > m_show_number)
                it.value()->removePoints(0, count - m_show_number);
        }
        if (!m_is_fix) {
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
        } else {
            auto axes = m_chart->axes(Qt::Horizontal);
            if (!axes.empty())
                axes.first()->setRange(m_min_x, m_max_x);
            axes = m_chart->axes(Qt::Vertical);
            if (!axes.empty())
                axes.first()->setRange(m_min_y, m_max_y);
        }

        int c = 0;
        QString info;
        for (auto it = m_table->begin(); it != m_table->end(); it++) {
            const QString& k = it.key();
            const auto* v = it.value();
            info += k + ": ";
            if (v->count() <= 0) { info += "; "; continue; };

            const QPointF& p = v->at(v->count() - 1);
            info += QString("%1, %2; ").arg(p.x()).arg(p.y());
            ++c;
        }
        m_text->setPos(std::max(0.0, m_chart->size().width() / 2 - c * m_SizeOfOneText),
                       m_chart->size().height() - 20);
        m_text->setText(info);
        m_chart_view->update();
    }
private:
    static constexpr int m_SizeOfOneText = 150;

    int m_show_number = 50;
    int m_right_side = 20;
    double m_min_x = 0.f;
    double m_max_x = 0.f;
    double m_min_y = 0.f;
    double m_max_y = 0.f;
    bool m_is_fix = false;

    QTimer* m_timer;
    DataTable* m_table; // 所有 数据点系列的 Map
    QtCharts::QLineSeries* m_y_zero;
    QtCharts::QChart* m_chart;
    QGraphicsSimpleTextItem* m_text;
    QtCharts::QChartView* m_chart_view;
private Q_SLOT:
    void updateViewSlot() { updateView(); };
};
}

#endif //ARMOR_AUTO_AIMING_VIEW_DYNAMIC_CHART_VIEW_H
