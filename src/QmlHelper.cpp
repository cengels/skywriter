#include <QQmlFile>
#include <QMetaProperty>

#include "QmlHelper.h"

QmlHelper::QmlHelper(QObject *parent) : QObject(parent)
{ }

Property::Property(QString name, QString type) : QObject(),
    m_name(name),
    m_type(type)
{ }

QString QmlHelper::toLocalFileString(const QUrl& url) const
{
    return QQmlFile::urlToLocalFileOrQrc(url);
}

QUrl QmlHelper::fromLocalFileString(const QString& url) const
{
    return QUrl::fromLocalFile(url);
}

QString QmlHelper::fileName(const QUrl& url) const
{
    return url.fileName();
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

QVector<Property*> QmlHelper::properties(QObject *item)
{
    const QMetaObject *meta = item->metaObject();

    QVector<Property*> list;
    for (int i = 0; i < meta->propertyCount(); i++)
    {
        QMetaProperty property = meta->property(i);
        list.push_back(new Property(property.name(), property.typeName()));
    }

    return list;
}
