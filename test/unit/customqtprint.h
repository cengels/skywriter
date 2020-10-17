#ifndef CUSTOMQTPRINT_H
#define CUSTOMQTPRINT_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <QObject>

// Defines custom print functions for Qt types to better integrate Qt with
// gtest and allow for more readable test assertion failure messages.

namespace {
    std::string toHexadecimal(const QString& string) {
        QByteArray byteArray = string.toUtf8();
        std::stringstream outerStream;

        for (const char& character : byteArray) {
            std::stringstream stream;
            stream << std::hex << std::setfill('0') << std::setw(4) << (int)character;
            outerStream << stream.str() << " ";
        }

        std::string result = outerStream.str();
        result.pop_back();

        return result;
    }
}

QT_BEGIN_NAMESPACE
inline void PrintTo(const QString &qString, ::std::ostream *os)
{
    *os << "\"" << qUtf8Printable(qString) << "\" (" << toHexadecimal(qString) << ")";
}
QT_END_NAMESPACE

#endif // CUSTOMQTPRINT_H
