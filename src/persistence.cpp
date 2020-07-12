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

bool persistence::commit(QFile& from, QFile& to, bool keepBackup) {
    from.close();
    to.close();

    const QString originalPath = to.fileName();
    const QString backupPath = originalPath + ".bak";

    if (to.exists() && !to.rename(backupPath)) {
        // Couldn't rename target file. Might be read-only or backupPath already
        // exists.
        if (QFile::exists(backupPath)) {
            if (!QFile::remove(backupPath) || !to.rename(backupPath)) {
                // Remove previous backup, rerun the rename.
                // If it still fails, no failsafe is possible. Return.
                return false;
            }
        } else {
            // If backupPath doesn't exist, the problem isn't possible to
            // determine. Return.
            return false;
        }
    }

    if (!from.rename(originalPath)) {
        // Couldn't rename to the originalPath. Source file might be read-only.
        // No failsafe is possible. Roll-back the target file and return.
        to.rename(originalPath);
        return false;
    }

    if (!keepBackup) {
        QFile::remove(backupPath);
    }

    return true;
}
