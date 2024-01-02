/**
 * @projectName IntelligentHanding
 * @file HikUi.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-09-03 17:09
 */

#ifndef HIKDRIVER_HIKUI_H
#define HIKDRIVER_HIKUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>

#include <HikDriver/HikDriver.h>
#include <vision_debug_widgets/slider.h>

class HikUi : public QWidget {
public:
    explicit HikUi(HikDriver& driver)
      : m_driver(&driver),
        layout(new QVBoxLayout),
        d_exposure_time(new vision_debug_widgets::Slider("exposure time", &m_exposure_time,
                                                         static_cast<int>(m_driver->getExposureTime().min),
                                                         50000)),
        d_gain(new vision_debug_widgets::Slider("gain", &m_gain,
                                                static_cast<int>(m_driver->getGain().min),
                                                static_cast<int>(m_driver->getGain().max))),
        m_exposure_time(static_cast<int>(m_driver->getExposureTime().current)),
        m_gain(static_cast<int>(m_driver->getGain().current))
    {
        this->setWindowTitle("Hid Debug Ui");
        // 自动曝光
        auto auto_exposure_time_off = new QRadioButton("off");
        auto auto_exposure_time_once = new QRadioButton("once");
        auto auto_exposure_time_continuous = new QRadioButton("continuous");
        auto_exposure_time_off->setChecked(true);
        this->m_driver->setAutoExposureTime(0);
        connect(d_exposure_time, &vision_debug_widgets::Slider::valChanged, [this](){
            this->m_driver->setExposureTime(static_cast<float>(m_exposure_time));
        });
        // 按钮组
        auto auto_exposure_time_group = new QButtonGroup(this);
        auto_exposure_time_group->addButton(auto_exposure_time_off);
        auto_exposure_time_group->addButton(auto_exposure_time_once);
        auto_exposure_time_group->addButton(auto_exposure_time_continuous);
        auto_exposure_time_group->setExclusive(true);
        // 按钮布局
        auto auto_exposure_time_layout = new QHBoxLayout;
        auto_exposure_time_layout->addWidget(auto_exposure_time_off);
        auto_exposure_time_layout->addWidget(auto_exposure_time_once);
        auto_exposure_time_layout->addWidget(auto_exposure_time_continuous);
        // 连接信号
        connect(auto_exposure_time_off, &QRadioButton::clicked, [this](bool checked){
            this->m_driver->setAutoExposureTime(0);
            connect(d_exposure_time, &vision_debug_widgets::Slider::valChanged, [this](){
                this->m_driver->setExposureTime(static_cast<float>(m_exposure_time));
            });
        });
        connect(auto_exposure_time_once, &QRadioButton::clicked, [this](bool checked){
            this->m_driver->setAutoExposureTime(1);
            disconnect(d_exposure_time, &vision_debug_widgets::Slider::valChanged, this, nullptr);  // TODO： disconnect 修改 定义 QMetaObject::Connection _connection;
        });
        connect(auto_exposure_time_continuous, &QRadioButton::clicked, [this](bool checked){
            this->m_driver->setAutoExposureTime(2);
            disconnect(d_exposure_time, &vision_debug_widgets::Slider::valChanged, this, nullptr);
        });
        // 自动增益
        auto auto_gain_off = new QRadioButton("off");
        auto auto_gain_once = new QRadioButton("once");
        auto auto_gain_continuous = new QRadioButton("continuous");
        // 初始值
        auto_gain_off->setChecked(true);
        this->m_driver->setAutoGain(0);
        connect(d_gain, &vision_debug_widgets::Slider::valChanged, [this](){
            this->m_driver->setGain(static_cast<float>(m_gain));
        });
        // 按钮组
        auto auto_gain_group = new QButtonGroup(this);
        auto_gain_group->addButton(auto_gain_off);
        auto_gain_group->addButton(auto_gain_once);
        auto_gain_group->addButton(auto_gain_continuous);
        auto_gain_group->setExclusive(true);
        // 按钮布局
        auto auto_gain_layout = new QHBoxLayout;
        auto_gain_layout->addWidget(auto_gain_off);
        auto_gain_layout->addWidget(auto_gain_once);
        auto_gain_layout->addWidget(auto_gain_continuous);
        // 连接信号
        connect(auto_gain_off, &QRadioButton::clicked, [this](bool checked){
            this->m_driver->setAutoGain(0);
            connect(d_gain, &vision_debug_widgets::Slider::valChanged, [this](){
                this->m_driver->setGain(static_cast<float>(m_gain));
            });
        });
        connect(auto_gain_once, &QRadioButton::clicked, [this](bool checked){
            this->m_driver->setAutoGain(1);
            disconnect(d_gain, &vision_debug_widgets::Slider::valChanged, this, nullptr);
        });
        connect(auto_gain_continuous, &QRadioButton::clicked, [this](bool checked){
            this->m_driver->setAutoGain(2);
            disconnect(d_gain, &vision_debug_widgets::Slider::valChanged, this, nullptr);
        });
        // 总布局
        layout->addWidget(d_exposure_time);
        layout->addLayout(auto_exposure_time_layout);
        layout->addWidget(d_gain);
        layout->addLayout(auto_gain_layout);
        this->setLayout(layout);
    }

private:
    HikDriver* m_driver;

    int m_exposure_time;
    int m_gain;

    vision_debug_widgets::Slider* d_exposure_time;  // 曝光时间
    vision_debug_widgets::Slider* d_gain;  // 增益

    QVBoxLayout* layout;
};
#endif //HIKDRIVER_HIKUI_H
