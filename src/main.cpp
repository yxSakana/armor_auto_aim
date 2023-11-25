#include <iostream>
#include <thread>

#ifdef DEBUG
#include <QApplication>
#else
#include <QCoreApplication>
#endif

#include <google_logger/google_logger.h>
#include <armor_auto_aim/armor_auto_aim.h>

int main(int argc, char* argv[]) {
    armor_auto_aim::google_log::initGoogleLogger(argc, argv);
#ifdef DEBUG
    QApplication q_core(argc, argv);
#else
    QCoreApplication q_core(argc, argv);
#endif

    armor_auto_aim::ArmorAutoAim armor_auto_aim;
    std::thread armor_auto_aim_thread([&armor_auto_aim]()-> void {
        armor_auto_aim.armorAutoAim();
    });
    armor_auto_aim_thread.detach();
//    armor_auto_aim_thread.detach();

#ifdef DEBUG
    return QApplication::exec();
#else
    return QCoreApplication::exec();
#endif
}
