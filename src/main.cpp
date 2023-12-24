#ifdef DEBUG
#include <QApplication>
#else
#include <QCoreApplication>
#endif
#include <QThread>

#include <google_logger/google_logger.h>
#include <armor_auto_aim/armor_auto_aim.h>
#include <armor_auto_aim/view.h>
#include <armor_auto_aim/serail.h>

int main(int argc, char* argv[]) {
    armor_auto_aim::google_log::initGoogleLogger(argc, argv);
#ifdef DEBUG
    QApplication q_core(argc, argv);
#else
    QCoreApplication q_core(argc, argv);
#endif
    QThread serial_thread;
    QThread view_thread;
    armor_auto_aim::ViewWork view_work;
    armor_auto_aim::SerialWork serial_work;
    armor_auto_aim::ArmorAutoAim auto_aim_thread("../config/config.yaml");

    QObject::connect(&auto_aim_thread, &armor_auto_aim::ArmorAutoAim::sendAimInfo,
                     &serial_work, &armor_auto_aim::SerialWork::sendAimInfo);
    QObject::connect(&serial_work, &armor_auto_aim::SerialWork::readyImuData,
                     &auto_aim_thread, &armor_auto_aim::ArmorAutoAim::pushImuData);
    QObject::connect(&auto_aim_thread, &armor_auto_aim::ArmorAutoAim::showFrame,
                     &view_work, &armor_auto_aim::ViewWork::showFrame);
    QObject::connect(&auto_aim_thread, &armor_auto_aim::ArmorAutoAim::viewEkfSign,
                     &view_work, &armor_auto_aim::ViewWork::viewEkf);
    QObject::connect(&auto_aim_thread, &armor_auto_aim::ArmorAutoAim::viewTimestampSign,
                     &view_work, &armor_auto_aim::ViewWork::viewTimestamp);

    view_work.moveToThread(&view_thread);
    serial_work.moveToThread(&serial_thread);

    serial_thread.start();
    view_thread.start();
    auto_aim_thread.start();

#ifdef DEBUG
    return QApplication::exec();
#else
    return QCoreApplication::exec();
#endif
}
