#ifndef QMLHELPER_H
#define QMLHELPER_H

#include <QObject>
#include <QUrl>

//! Contains methods that will be globally accessible in QML.
class QmlHelper : public QObject
{
    Q_OBJECT

    public:
        explicit QmlHelper(QObject *parent = nullptr);

    Q_INVOKABLE QString toLocalFileString(const QUrl& url) const;
    Q_INVOKABLE QUrl fromLocalFileString(const QString& url) const;
    template<typename T>
    T clamp(const T value, const T min, const T max) const;

};

#endif // QMLHELPER_H
