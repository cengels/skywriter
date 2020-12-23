#ifndef QMLHELPER_H
#define QMLHELPER_H

#include <QMetaProperty>
#include <QColor>
#include <QObject>
#include <QUrl>

class Property : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(QString type MEMBER m_type NOTIFY typeChanged)

    Q_SIGNALS:
        void nameChanged();
        void typeChanged();

    public:
        explicit Property(QString name, QString type);

    private:
        QString m_name;
        QString m_type;
};

//! Contains methods that will be globally accessible in QML.
class QmlHelper : public QObject
{
    Q_OBJECT

    public:
        explicit QmlHelper(QObject *parent = nullptr);

    Q_INVOKABLE QString toLocalFileString(const QUrl& url) const;
    Q_INVOKABLE QUrl fromLocalFileString(const QString& url) const;
    Q_INVOKABLE QString fileName(const QUrl& url) const;
    template<typename T>
    T clamp(const T& value, const T& min, const T& max) const;
    Q_INVOKABLE bool isNumeric(const QString& string) const;
    Q_INVOKABLE bool isValidHex(const QString& string) const;
    Q_INVOKABLE QColor colorFromHex(const QString& string) const;
    //! Returns a hash of all properties in the QObject along with their types.
    Q_INVOKABLE QVector<Property*> properties(QObject *item);
};

#endif // QMLHELPER_H
