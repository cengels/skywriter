#include <QFile>
#include <QString>
#include <QDataStream>
#include <QTextStream>
#include <QGuiApplication>
#include <QDebug>

#include "ErrorManager.h"

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

namespace persistence {
    const QLatin1String format_markdown = QLatin1String("md");
    const QLatin1String format_html = QLatin1String("htm");
    const QLatin1String format_bak = QLatin1String("bak");
    //! Gets the path where settings and application data should be stored.
    QString documentsPath();
    //! Replaces the target file with the specified source file. If at any
    //! point of the process an error occurs, the function will return false
    //! and the files will be rolled back so that the target file is not
    //! corrupted.
    bool commit(QFile& from, QFile& to, bool keepBackup = true);

    //! Safely overwrites a file by first writing the contents to a temporary
    //! file on the disk and then swapping out the two files. If at any point
    //! of this process an error occurs, all changes are rolled back and the
    //! function returns false.
    template<typename Stream>
    bool overwrite(QFile& file, std::function<bool(Stream&)> callback, bool keepBackup = true)
    {
        static_assert(std::is_same<QTextStream, Stream>() || std::is_same<QDataStream, Stream>(), "Must pass either a QTextStream or QDataStream.");

        const QString tempName = file.fileName() + ".tmp";

        QFile outputFile(tempName);

        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            emit ErrorManager::instance()->error(QGuiApplication::tr("Could not open file") + tempName);

            return false;
        }

        Stream out(&outputFile);

        if (!callback(out)) {
            return false;
        }

        outputFile.close();

        return commit(outputFile, file, keepBackup);
    }
}

#endif // PERSISTENCE_H
