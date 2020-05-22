#include <QObject>
#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QTimer>

#include "ProgressTracker.h"
#include "ProgressItem.h"
#include "../persistence.h"

namespace {
    QString m_progressPath = QString();
    QString m_tempProgressPath = QString();
    const QString progressFileName = "/progress2.csv";
    QString progressPath()
    {
        if (m_progressPath.isNull()) {
            m_progressPath = persistence::documentsPath() + progressFileName;
        }

        return m_progressPath;
    }

    QString tempProgressPath()
    {
        if (m_tempProgressPath.isNull()) {
            m_tempProgressPath = persistence::documentsPath() + "/progress.temp";
        }

        return m_tempProgressPath;
    }

    //! Gets the date component of the specified QDateTime. If the time
    //! is before the specified dailyReset, the previous day is returned.
    const QDate getAdjustedDate(const QDateTime& dateTime, const QTime& adjustBy)
    {
        return adjustBy.isValid() && dateTime.time() < adjustBy
            ? dateTime.date().addDays(-1)
            : dateTime.date();
    }

    bool safelyReplaceFile(QFile& from, QFile& to) {
        from.close();
        to.close();

        const QString originalPath = to.fileName();
        const QString backupPath = originalPath + ".bak";

        if (!to.rename(backupPath)) {
            if (QFile::exists(backupPath)) {
                if (!QFile::remove(backupPath) || !to.rename(backupPath)) {
                    return false;
                }
            } else {
                return false;
            }
        }

        if (!from.rename(originalPath)) {
            to.rename(originalPath);
            return false;
        }

        return true;
    }
}

ProgressTracker::ProgressTracker(QObject *parent)
    : QObject(parent)
    , m_progressToday(0)
    , m_items(),
      m_items_to_save()
    , m_activeProgressItem(nullptr)
    , m_maximumIdleMinutes(0)
    , m_autosaveMinutes(0)
    , m_dailyReset()
    , m_fileUrl()
    , m_hasRunningTimer(false)
{
}

int ProgressTracker::progressToday() const
{
    return this->m_progressToday;
}

QList<ProgressItem*> ProgressTracker::items() const
{
    return this->m_items;
}

int ProgressTracker::maximumIdleMinutes() const
{
    return this->m_maximumIdleMinutes;
}

void ProgressTracker::setMaximumIdleMinutes(const int minutes)
{
    this->m_maximumIdleMinutes = minutes;
    emit maximumIdleMinutesChanged();
}

int ProgressTracker::autosaveMinutes() const
{
    return this->m_autosaveMinutes;
}

void ProgressTracker::setAutosaveMinutes(const int minutes)
{
    this->m_autosaveMinutes = minutes;
    emit autosaveMinutesChanged();
}

QTime ProgressTracker::dailyReset() const {
    return this->m_dailyReset;
}

void ProgressTracker::setDailyReset(const QTime& time) {
    this->m_dailyReset = time;
    emit dailyResetChanged();
}

QUrl ProgressTracker::fileUrl() const
{
    return this->m_fileUrl;
}

void ProgressTracker::addProgress(const int words)
{
    if (words == 0) {
        return;
    }

    bool skipIdleCheck = false;
    const QDateTime& now = QDateTime::currentDateTime();

    if (this->m_activeProgressItem != nullptr
            && this->m_activeProgressItem->end().secsTo(now) > this->m_maximumIdleMinutes * 60) {
        this->m_activeProgressItem = nullptr;
        skipIdleCheck = true;
    }

    if (this->m_activeProgressItem == nullptr) {
        if (!skipIdleCheck
                && !this->m_items.isEmpty()
                && this->m_items.constLast()->fileUrl() == m_fileUrl
                && this->m_items.constLast()->end().secsTo(now) <= this->m_maximumIdleMinutes * 60) {
            this->m_activeProgressItem = this->m_items.last();
            this->m_items_to_save.append(m_activeProgressItem);
        } else {
            ProgressItem* progressItem = new ProgressItem(this);
            progressItem->setFileUrl(this->m_fileUrl);
            progressItem->setStart(QDateTime::currentDateTime());
            this->m_items.append(progressItem);
            this->m_items_to_save.append(progressItem);
            this->m_activeProgressItem = progressItem;
            emit itemsChanged();
        }
    }

    this->m_activeProgressItem->setWords(this->m_activeProgressItem->words() + words);
    this->m_activeProgressItem->setEnd(QDateTime::currentDateTime());

    this->m_progressToday += words;
    emit progressTodayChanged();

    if (!m_hasRunningTimer) {
        QTimer::singleShot(m_autosaveMinutes * 60000, this, SLOT(autosave()));
        m_hasRunningTimer = true;
    }
}

void ProgressTracker::autosave() {
    if (!m_hasRunningTimer) {
        // Consider the autosave canceled and abort the operation.
        return;
    }

    this->save();

    m_hasRunningTimer = false;
}

void ProgressTracker::changeActiveFile(const QUrl& fileUrl)
{
    this->m_activeProgressItem = nullptr;
    this->m_fileUrl = fileUrl;

    emit fileUrlChanged();
}

void ProgressTracker::renameActiveFile(const QUrl& fileUrl)
{
    this->m_activeProgressItem->setFileUrl(fileUrl);
    QString previousUrl = m_fileUrl.toLocalFile();
    this->m_fileUrl = fileUrl;

    QFile inputFile(progressPath());
    QFile outputFile(tempProgressPath());
    if (inputFile.open(QIODevice::ReadWrite | QIODevice::Text) && outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream input(&inputFile);
        QTextStream output(&outputFile);

        while (!input.atEnd()) {
            QString line = input.readLine();
            if (!line.isEmpty()) {
                if (line.startsWith(previousUrl)) {
                    auto split = line.split(',');
                    split.replace(0, previousUrl);
                    output << split.join(',') << '\n';
                } else {
                    output << line << '\n';
                }
            }
        }

        bool success = safelyReplaceFile(outputFile, inputFile);

        if (!success) {
            qCritical() << "Failed to rename file.";
        }
    } else {
        qCritical() << "Failed to rename file. Couldn't open file.";
    }

    emit fileUrlChanged();
}

QList<ProgressItem*> ProgressTracker::read(int year, int month, const QTime& adjustBy, QObject* parent)
{
    QList<ProgressItem*> list;
    QFile file(progressPath());

    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                ProgressItem* item = ProgressItem::fromCsv(line);
                const QDate& date = getAdjustedDate(item->start(), adjustBy);

                if (date.year() == year && (month == 0 || date.month() == month)) {
                    item->setParent(parent);
                    list.append(item);
                }
            }
        }

        file.close();
    } else {
        qCritical("Couldn't open progress.csv.");
    }

    return list;
}

void ProgressTracker::load()
{
    const QDate& today = getAdjustedDate(QDateTime::currentDateTime(), m_dailyReset);

    if (!this->m_items.isEmpty()) {
        qDeleteAll(this->m_items);
        this->m_items.clear();
    }

    this->m_items = read(today.year(), today.month(), m_dailyReset);

    const auto end = this->m_items.crend();

    for (auto i = this->m_items.crbegin(); i != end; i++) {
        const QDate& start = getAdjustedDate((*i)->start(), m_dailyReset);

        if (start != today) {
            break;
        }

        m_progressToday += (*i)->words();
    }

    emit itemsChanged();

    if (m_progressToday != 0) {
        emit progressTodayChanged();
    }
}

void ProgressTracker::save()
{
    if (!m_items_to_save.isEmpty()) {
        QFile inputFile(progressPath());
        QFile outputFile(tempProgressPath());
        if (inputFile.open(QIODevice::ReadWrite | QIODevice::Text) && outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream input(&inputFile);
            QTextStream output(&outputFile);

            while (!input.atEnd()) {
                const QString line = input.readLine();
                const ProgressItem* matchingItem = nullptr;

                // Try to match items to existing lines and overwrite them
                // if found

                foreach (ProgressItem* item, m_items_to_save) {
                    if (item->isCsv(line)) {
                        matchingItem = item;
                        m_items_to_save.removeOne(item);
                        break;
                    }
                }

                if (matchingItem != nullptr) {
                    if (matchingItem->words() != 0) {
                        output << matchingItem->toCsv() << '\n';
                    }
                } else {
                    output << line << '\n';
                }
            }

            // Append the items that could not be matched

            foreach (ProgressItem* item, m_items_to_save) {
                if (item->words() != 0) {
                    output << item->toCsv() << '\n';
                }
            }

            bool success = safelyReplaceFile(outputFile, inputFile);

            if (!success) {
                qCritical() << "Failed to rename file.";
            }

            m_items_to_save.clear();

            m_hasRunningTimer = false;
        } else {
            qCritical("Couldn't open progress.csv.");
        }
    }
}
