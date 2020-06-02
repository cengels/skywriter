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
