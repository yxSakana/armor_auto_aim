/**
 * @file QtCompatibilityLayer.h
 * @author yao
 * @date 2021年01月13日
 * @brief Qt类型兼容层，以后有需要再拓展
 */

#ifndef KDROBOTCPPLIBS_QTCOMPATIBILITYLAYER_H
#define KDROBOTCPPLIBS_QTCOMPATIBILITYLAYER_H

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <QtCore>
#include <QtNetwork/QHostAddress>

template<class _Traits>
inline std::basic_ostream<char, _Traits> &
operator<<(std::basic_ostream<char, _Traits> &os, const QString &c) {
    return os << c.toLocal8Bit().toStdString();
}

template<class _Traits>
inline std::basic_ostream<char, _Traits> &
operator<<(std::basic_ostream<char, _Traits> &os, const QByteArray &c) {
    return os << c.toStdString();
}

template<class _Traits>
inline std::basic_ostream<char, _Traits> &
operator<<(std::basic_ostream<char, _Traits> &os, const QJsonObject &c) {
    return os << QJsonDocument(c);
}

template<class _Traits>
inline std::basic_ostream<char, _Traits> &
operator<<(std::basic_ostream<char, _Traits> &os, const QJsonDocument &c) {
    return os << c.toJson();
}

template<class _Traits>
inline std::basic_ostream<char, _Traits> &
operator<<(std::basic_ostream<char, _Traits> &os, const QHostAddress &c) {
    return os << c.toString();
}

#endif
