#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <QObject>

class ErrorManager : public QObject
{
    Q_OBJECT

    public:
        explicit ErrorManager(QObject *parent = nullptr);

        static ErrorManager* instance();

    Q_SIGNALS:
        void error(const QString& message);
        void warning(const QString& message);
        void info(const QString& message);
};

#endif // ERRORMANAGER_H
