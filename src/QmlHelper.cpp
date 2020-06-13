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

template<typename T>
T QmlHelper::clamp(const T value, const T min, const T max) const
{
    return qMin(max, qMax(min, value));
}
