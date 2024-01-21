/**
 * @projectName armor_auto_aim
 * @file slider_widget.cpp
 * @brief 
 * @author yx
 * @date 2023-07-28 20:12
 */

#ifndef VISION_DEBUG_WIDGETS_SLIDER_WIDGET_H
#define VISION_DEBUG_WIDGETS_SLIDER_WIDGET_H

#include <QMainWindow>
#include <vision_debug_widgets/slider.h>

namespace vision_debug_widgets {
class SliderWidget : public QMainWindow {
public:
    explicit SliderWidget(QWidget* parent=nullptr)
        : QMainWindow(parent),
          central_widget(new QWidget(this)),
          layout(new QVBoxLayout) {
        this->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        this->central_widget->setLayout(layout);
        this->setCentralWidget(central_widget);
    }

    void addSlider(const QString& name, int *val, const int min, const int max) {
        auto slider = new Slider(name, val, min, max, this);
        this->layout->addWidget(slider);
    }

    void addSlider(const QString &name, double *val, const int min, const int max) {
        auto slider = new Slider(name, val, min, max, this);
        this->layout->addWidget(slider);
    }
private:
    QWidget* central_widget;
    QVBoxLayout* layout;
};
} // vision_debug_widgets
#endif //VISION_DEBUG_WIDGETS_SLIDER_WIDGET_H
