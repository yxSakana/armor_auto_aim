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
    armor_auto_aim::ArmorAutoAim armor_auto_aim("../config/config.yaml");
    armor_auto_aim.start();

#ifdef DEBUG
    return QApplication::exec();
#else
    return QCoreApplication::exec();
#endif
}
