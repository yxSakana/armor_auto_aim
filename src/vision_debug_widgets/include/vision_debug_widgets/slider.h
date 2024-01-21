/**
 * @projectName test_qtAndMat
 * @file Slider.h
 * @brief debug 滑块
 * @author yx
 * @date 2023-07-26 16:29
 */

#ifndef VISION_DEBUG_WIDGETS_SLIDER_H
#define VISION_DEBUG_WIDGETS_SLIDER_H

#include <utility>

#include <qdebug.h>
#include <QString>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QIntValidator>

namespace vision_debug_widgets {
class Slider : public QWidget {
    Q_OBJECT
public:
    Slider(QString name, int* val, const int min, const int max, QWidget* parent=nullptr)
            : slider(new QSlider(Qt::Horizontal, parent)),
              val_name(std::move(name)),
              val_edit(new QLineEdit(this)),
              name_label(new QLabel(val_name, this)),
              val_label(new QLabel( this)),
              slider_box(new QHBoxLayout) {
        this->m_val = val;

        /* 值 滑动条 */
        this->slider->setRange(min, max);
        this->slider->setValue(*val);

        /* 值 编辑栏 */
        this->val_edit->setValidator(new QIntValidator(min, max, this));  // 限制范围

        /* 布局 */
        this->slider_box->addWidget(name_label);
        this->slider_box->addWidget(val_label);
        this->slider_box->addWidget(slider);
        this->slider_box->addWidget(val_edit);
        this->slider_box->setStretch(0, 1);
        this->slider_box->setStretch(1, 1);
        this->slider_box->setStretch(2, 20);
        this->slider_box->setStretch(3, 1);
        this->setLayout(slider_box);

        this->finishValShow();

        connect(this->slider, &QSlider::valueChanged, this, &Slider::onValChanged);
        connect(this->val_edit, &QLineEdit::editingFinished, this, &Slider::onEditValChanged);
    }

    Slider(QString name, double* val, const int min, const int max, QWidget* parent=nullptr)
            : slider(new QSlider(Qt::Horizontal, parent)),
              val_name(std::move(name)),
              val_edit(new QLineEdit(this)),
              name_label(new QLabel(val_name, this)),
              val_label(new QLabel( this)),
              slider_box(new QHBoxLayout) {
        this->m_dval = val;

        /* 值 滑动条 */
        this->slider->setRange(min, max);
        this->slider->setValue(*val);

        /* 值 编辑栏 */
        this->val_edit->setValidator(new QIntValidator(min, max, this));  // 限制范围

        /* 布局 */
        this->slider_box->addWidget(name_label);
        this->slider_box->addWidget(val_label);
        this->slider_box->addWidget(slider);
        this->slider_box->addWidget(val_edit);
        this->slider_box->setStretch(0, 1);
        this->slider_box->setStretch(1, 1);
        this->slider_box->setStretch(2, 20);
        this->slider_box->setStretch(3, 1);
        this->setLayout(slider_box);

        this->finishValShow();

        connect(this->slider, &QSlider::valueChanged, this, &Slider::onValChangedDouble);
        connect(this->val_edit, &QLineEdit::editingFinished, this, &Slider::onEditValChangedDouble);
    }

    [[nodiscard]] const QString& getName() const { return val_name; }

    void setEnable(bool status) {
        this->slider->setEnabled(status);
        this->val_edit->setEnabled(status);
    }
signals:
    void valChanged();
private:
    QString val_name;  // 值的名称  TODO: 可以用来做 JSON 的 Key
    QSlider* slider;  // 滑动条
    QLineEdit* val_edit;  // 值编辑栏

    QLabel *name_label, *val_label;  // 显示值的名称 和 当前值/最大值
    QHBoxLayout* slider_box;
    int* m_val;
    double* m_dval;

    /**
     * 更新 val_label、slider、val_edit 显示
     */
    inline void finishValShow() {
        this->val_label->setText("~~  " + QString::number(*this->m_val) + "/" + QString::number(this->slider->maximum()));
        this->slider->setValue(*this->m_val);
        this->val_edit->setText(QString::number(*this->m_val));
    }

    inline void finishValShowDouble() {
        this->val_label->setText(("-- " + QString::number(*this->m_dval) + "/" + QString::number(this->slider->maximum())));
        this->slider->setValue(*this->m_dval);
        this->val_edit->setText(QString::number(*this->m_dval));
    }
private slots:
    /**
     * @brief[slots] 滑动条值更改
     * @param value
     */
    void onValChanged(int value) {
        *this->m_val = value;
        finishValShow();
        emit valChanged();
    }

    void onValChangedDouble(int value) {
        *this->m_dval = value * 1.0;
        finishValShowDouble();
        emit valChanged();
    }

    /**
     * 值编辑栏值更改 [slots]
     */
    void onEditValChanged() {
        int val = this->val_edit->text().toInt();
        *this->m_val = val;
        finishValShow();
        emit valChanged();
    }

    void onEditValChangedDouble() {
        int val = this->val_edit->text().toInt();
        *this->m_dval = val;
        finishValShowDouble();
        emit valChanged();
    }
};
}
#endif //VISION_DEBUG_WIDGETS_SLIDER_H
