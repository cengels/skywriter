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
    //! Gets the path where settings and application data should be stored.
    QString documentsPath();
    //! Replaces the target file with the specified source file. If at any
    //! point of the process an error occurs, the function will return false
    //! and the files will be rolled back so that the target file is not
    //! corrupted.
    bool commit(QFile& from, QFile& to);

    //! Safely overwrites a file by first writing the contents to a temporary
    //! file on the disk and then swapping out the two files. If at any point
    //! of this process an error occurs, all changes are rolled back and the
    //! function returns false.
    template<typename Stream>
    bool overwrite(QFile& file, std::function<bool(Stream&)> callback)
    {
        static_assert(std::is_same<QTextStream, Stream>() || std::is_same<QDataStream, Stream>(), "Must pass either a QTextStream or QDataStream.");

        const QString tempName = file.fileName() + ".tmp";

        QFile outputFile(tempName);

        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            emit ErrorManager::instance()->error(QGuiApplication::tr("Could not open file") + tempName);

            return false;
        }

        Stream out(&outputFile);

        if (std::is_same<QTextStream, Stream>()) {
            out.setCodec("UTF-8");
        }

        if (!callback(out)) {
            return false;
        }

        outputFile.close();

        return commit(outputFile, file);
    }
}

#endif // PERSISTENCE_H
