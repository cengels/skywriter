#include <QQmlFile>

#include "QmlHelper.h"

QmlHelper::QmlHelper(QObject *parent) : QObject(parent)
{

}

QString QmlHelper::toLocalFileString(const QUrl& url) const
{
    return QQmlFile::urlToLocalFileOrQrc(url);
}

QUrl QmlHelper::fromLocalFileString(const QString& url) const
{
    return QUrl::fromLocalFile(url);
}

bool QmlHelper::isNumeric(const QString& string) const
{
    for (const QChar character : string) {
        if (!character.isNumber() && character != '.') {
            return false;
        }
    }

    return true;
}

bool QmlHelper::isValidHex(const QString& string) const
{
    // Valid: #000 or #000000
    const int length = string.length();
    if (length != 4 && length != 7) {
        return false;
    }

    if (string.at(0) != '#') {
        return false;
    }

    for (int i = 1; i < length; i++) {
        if (!string.at(i).isDigit()) {
            return false;
        }
    }

    return true;
}

QColor QmlHelper::colorFromHex(const QString& string) const
{
    if (!isValidHex(string)) {
        return QColor();
    }

    return QColor(string);
}

template<typename T>
T QmlHelper::clamp(const T& value, const T& min, const T& max) const
{
    return qBound(min, value, max);
}
