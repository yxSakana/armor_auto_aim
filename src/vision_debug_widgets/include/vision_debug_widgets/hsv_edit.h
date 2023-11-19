/**
 * @projectName IntelligentHanding
 * @file DHsvEdit.cpp
 * @brief 
 * @author yx
 * @date 2023-09-02 08:12
 */

#ifndef INTELLIGENTHANDING_DHSVEDIT_H
#define INTELLIGENTHANDING_DHSVEDIT_H

#ifdef NO_USING_PACKAGE

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace vision_debug_ui {
    class DHsvEdit : public QWidget
    {
        Q_OBJECT
    public:
        DHsvEdit(const std::string& name,
                 HsvThreshold& threshold, QWidget* parent = nullptr)
                : QWidget(parent),
                  name_label(new QLabel(QString::fromStdString(name) + ": ")),
                  h_edit_l(new QSpinBox(this)), h_edit_h(new QSpinBox(this)),
                  s_edit_l(new QSpinBox(this)), s_edit_h(new QSpinBox(this)),
                  v_edit_l(new QSpinBox(this)), v_edit_h(new QSpinBox(this)),
                  h_layout(new QHBoxLayout)
        {
            h_edit_l->setRange(0, 255);
            h_edit_h->setRange(0, 255);
            s_edit_l->setRange(0, 255);
            s_edit_h->setRange(0, 255);
            v_edit_l->setRange(0, 255);
            v_edit_h->setRange(0, 255);

            h_edit_l->setValue(static_cast<int>(threshold.lower[0]));
            h_edit_h->setValue(static_cast<int>(threshold.upper[0]));
            s_edit_l->setValue(static_cast<int>(threshold.lower[1]));
            s_edit_h->setValue(static_cast<int>(threshold.upper[1]));
            v_edit_l->setValue(static_cast<int>(threshold.lower[2]));
            v_edit_h->setValue(static_cast<int>(threshold.upper[2]));

            h_layout->addWidget(name_label);
            auto h_label = new QLabel("H");
            h_layout->addWidget(h_label);
            h_layout->addWidget(h_edit_l);
            h_layout->addWidget(h_edit_h);
            auto s_label = new QLabel("S");
            h_layout->addWidget(s_label);
            h_layout->addWidget(s_edit_l);
            h_layout->addWidget(s_edit_h);
            auto v_label = new QLabel("V");
            h_layout->addWidget(v_label);
            h_layout->addWidget(v_edit_l);
            h_layout->addWidget(v_edit_h);
            this->setLayout(h_layout);

            connect(h_edit_l, QOverload<int>::of(&QSpinBox::valueChanged), [this, &threshold](int val){
                h_edit_l->setValue(val);
                threshold.lower[0] = val * 1.0;
            });
            connect(h_edit_h, QOverload<int>::of(&QSpinBox::valueChanged), [this, &threshold](int val){
                h_edit_h->setValue(val);
                threshold.upper[0] = val * 1.0;
            });
            connect(s_edit_l, QOverload<int>::of(&QSpinBox::valueChanged), [this, &threshold](int val){
                s_edit_l->setValue(val);
                threshold.lower[1] = val * 1.0;
            });
            connect(s_edit_h, QOverload<int>::of(&QSpinBox::valueChanged), [this, &threshold](int val){
                s_edit_h->setValue(val);
                threshold.upper[1] = val * 1.0;
            });
            connect(v_edit_l, QOverload<int>::of(&QSpinBox::valueChanged), [this, &threshold](int val){
                v_edit_l->setValue(val);
                threshold.lower[2] = val * 1.0;
            });
            connect(v_edit_h, QOverload<int>::of(&QSpinBox::valueChanged), [this, &threshold](int val){
                v_edit_h->setValue(val);
                threshold.upper[2] = val * 1.0;
            });
        }

    private:
        QLabel* name_label;
        QSpinBox* h_edit_l;
        QSpinBox* h_edit_h;
        QSpinBox* s_edit_l;
        QSpinBox* s_edit_h;
        QSpinBox* v_edit_l;
        QSpinBox* v_edit_h;
        QHBoxLayout* h_layout;

    };

    class DHsvEditWin : public QWidget {
        Q_OBJECT
    public:
        explicit DHsvEditWin(QWidget* parent = nullptr)
            : QWidget(parent),
              m_save_pb(new QPushButton("save", this)),
              edit_layout(new QVBoxLayout),
              pb_layout(new QHBoxLayout),
              layout(new QVBoxLayout)
        {
            pb_layout->addSpacing(40);
            pb_layout->addWidget(m_save_pb);
            pb_layout->addSpacing(40);
            layout->addLayout(edit_layout);
            layout->addLayout(pb_layout);
            this->setLayout(layout);
        }

        void addHsvEdit(const std::string& name, HsvThreshold& threshold)
        {
            auto _ = new DHsvEdit(name, threshold);
            edit_layout->addWidget(_);
        }

        template<class T>
        void connectSaveSign(const T& other)
        {
            connect(m_save_pb, &QPushButton::clicked, &other, &T::save);
        }

    private:
        QPushButton* m_save_pb;

        QVBoxLayout* edit_layout;
        QHBoxLayout* pb_layout;  // 各种功能的按钮的布局
        QVBoxLayout* layout;
    };
}

#endif

#endif //INTELLIGENTHANDING_DHSVEDIT_H
