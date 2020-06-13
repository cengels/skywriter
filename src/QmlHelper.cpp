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

bool QmlHelper::isNumeric(const QString string) const
{
    for (const QChar character : string) {
        if (!character.isNumber() && character != '.') {
            return false;
        }
    }

    return true;
}

template<typename T>
T QmlHelper::clamp(const T value, const T min, const T max) const
{
    return qMin(max, qMax(min, value));
}
