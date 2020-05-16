#include <QString>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include "persistence.h"

namespace {
    QString m_documentsPath = QString();
}

QString persistence::documentsPath()
{
    // This method uses QStandardPaths to retrieve the AppDataLocation.
    // However, since the AppDataLocation always appends the organization
    // name if it is defined, and it's not possible to not define the
    // organization name or QSettings will break, it's necessary to manually
    // remove it from the QString here.

    // The rationale for removing it is that I (cengels) am the sole developer
    // of this application. It is highly unusual to store application data
    // in a subfolder specific to the developer.

    if (m_documentsPath.isNull()) {
        const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QStringList splitPath = path.split('/');

        const int index = splitPath.length() - 2;

        if (splitPath.at(index) == QCoreApplication::organizationName()) {
            splitPath.removeAt(index);
        }

        m_documentsPath = splitPath.join('/');
    }

    // Create directory if it doesn't exist

    QDir dir(m_documentsPath);
    if (!dir.exists()) {
        qDebug() << "Creating documents path:" << m_documentsPath;
        dir.mkpath(".");
    }

    return m_documentsPath;
}
