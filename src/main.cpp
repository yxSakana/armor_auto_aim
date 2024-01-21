/**
 * @project_name armor_auto_aiming
 * @file main.cpp
 * @brief
 * @author yx
 * @date 2023-10-28 21:13:26
 */

#ifdef DEBUG
#include <QApplication>
#else
#include <QCoreApplication>
#endif
#include <QThread>

#include <google_logger/google_logger.h>
#include <armor_auto_aim/armor_auto_aim.h>
#include <armor_auto_aim/view_work.h>
#include <armor_auto_aim/serail_work.h>

int main(int argc, char* argv[]) {
    armor_auto_aim::google_log::initGoogleLogger(argc, argv);
#ifdef DEBUG
    QApplication q_core(argc, argv);
#else
    QCoreApplication q_core(argc, argv);
#endif
    QThread serial_thread;
#ifdef DEBUG
    QThread view_thread;
    armor_auto_aim::ViewWork view_work;
#endif
    std::string cfg_filename = argc == 2? argv[1]: "../config/infantry.yaml";
    armor_auto_aim::SerialWork serial_work;
    armor_auto_aim::ArmorAutoAim auto_aim_thread(cfg_filename);
    auto_aim_thread.setSerialWork(&serial_work);
    std::unique_ptr<armor_auto_aim::ArmorAutoAim> auto_aim_thread_2;
    if (argc == 3) {
        auto_aim_thread_2 = std::make_unique<armor_auto_aim::ArmorAutoAim>(argv[2]);
        QObject::connect(auto_aim_thread_2.get(), &armor_auto_aim::ArmorAutoAim::sendAimInfo,
                         &serial_work, &armor_auto_aim::SerialWork::sendAimInfo);
        QObject::connect(&serial_work, &armor_auto_aim::SerialWork::readyImuData,
                         auto_aim_thread_2.get(), &armor_auto_aim::ArmorAutoAim::pushImuData);
    }
#ifdef DEBUG
    auto_aim_thread.setViewWork(&view_work);
#endif

    QObject::connect(&auto_aim_thread, &armor_auto_aim::ArmorAutoAim::sendAimInfo,
                     &serial_work, &armor_auto_aim::SerialWork::sendAimInfo);
    QObject::connect(&serial_work, &armor_auto_aim::SerialWork::readyImuData,
                     &auto_aim_thread, &armor_auto_aim::ArmorAutoAim::pushImuData);
#ifdef DEBUG
    QObject::connect(&auto_aim_thread, &armor_auto_aim::ArmorAutoAim::showFrame,
                     &view_work, &armor_auto_aim::ViewWork::showFrame);
    QObject::connect(&auto_aim_thread, &armor_auto_aim::ArmorAutoAim::viewEkfSign,
                     &view_work, &armor_auto_aim::ViewWork::viewEkf);
    QObject::connect(&auto_aim_thread, &armor_auto_aim::ArmorAutoAim::viewTimestampSign,
                     &view_work, &armor_auto_aim::ViewWork::viewTimestamp);
    QObject::connect(&serial_work, &armor_auto_aim::SerialWork::showThreadIdSignal,
                     &view_work, &armor_auto_aim::ViewWork::showThreadId);
#endif
    QObject::connect(&serial_work, &armor_auto_aim::SerialWork::showThreadIdSignal,
                     &serial_work, &armor_auto_aim::SerialWork::showThreadId);
#ifdef DEBUG
    view_work.moveToThread(&view_thread);
    view_thread.start();
#endif
    serial_work.moveToThread(&serial_thread);
    serial_thread.start();
    auto_aim_thread.start();
    if (auto_aim_thread_2 != nullptr) auto_aim_thread_2->start();
#ifdef DEBUG
//    view_work.show();
#endif
    LOG(INFO) << fmt::format("main_thread: {};", QThread::currentThreadId());
    emit serial_work.showThreadIdSignal();
#ifdef DEBUG
    return QApplication::exec();
#else
    return QCoreApplication::exec();
#endif
}
