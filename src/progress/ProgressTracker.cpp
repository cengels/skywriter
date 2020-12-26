#include <QObject>
#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QTimer>

#include "ProgressTracker.h"
#include "ProgressItem.h"
#include "../persistence.h"
#include "../ErrorManager.h"

namespace {
    QString m_progressPath = QString();
    const QString progressFileName = "/progress.csv";
    QString progressPath()
    {
        if (m_progressPath.isNull()) {
            m_progressPath = persistence::documentsPath() + progressFileName;
        }

        return m_progressPath;
    }

    //! Gets the date component of the specified QDateTime. If the time
    //! is before the specified dailyReset, the previous day is returned.
    const QDate getAdjustedDate(const QDateTime& dateTime, const QTime& adjustBy)
    {
        return adjustBy.isValid() && dateTime.time() < adjustBy
            ? dateTime.date().addDays(-1)
            : dateTime.date();
    }
}

ProgressTracker::ProgressTracker(QObject *parent)
    : QObject(parent)
    , m_progressToday(0)
    , m_items(),
      m_items_to_save()
    , m_activeProgressItem(nullptr)
    , m_minimumIdleMinutes(0)
    , m_dailyReset()
    , m_fileUrl()
    , m_skipIdleCheck(false)
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

int ProgressTracker::minimumIdleMinutes() const
{
    return this->m_minimumIdleMinutes;
}

void ProgressTracker::setMinimumIdleMinutes(const int minutes)
{
    this->m_minimumIdleMinutes = minutes;
    emit minimumIdleMinutesChanged();
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

    bool skipIdleCheck = this->m_skipIdleCheck;
    this->m_skipIdleCheck = false;
    const QDateTime& now = QDateTime::currentDateTime();

    if (this->m_activeProgressItem != nullptr
            && this->m_activeProgressItem->end().secsTo(now) > this->m_minimumIdleMinutes * 60) {
        this->m_activeProgressItem = nullptr;
        skipIdleCheck = true;
    }

    if (this->m_activeProgressItem == nullptr) {
        if (!skipIdleCheck
                && !this->m_items.isEmpty()
                && this->m_items.constLast()->fileUrl() == m_fileUrl
                && this->m_items.constLast()->end().secsTo(now) <= this->m_minimumIdleMinutes * 60) {
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

//    const int previousProgress = m_activeProgressItem->words();
    this->m_activeProgressItem->setWords(this->m_activeProgressItem->words() + words);

    // This should be user-configurable.
    // It means that, if a user adds some words but then removes them all
    // and starts again, the time will reset. However, if a user were to
    // remove all words and then hit undo, the ProgressTracker would think
    // they wrote all of those words in an instant.
//    if (previousProgress == 0) {
//        m_activeProgressItem->setStart(QDateTime::currentDateTime());
//    }

    this->m_activeProgressItem->setEnd(QDateTime::currentDateTime());

    this->m_progressToday += words;
    emit progressTodayChanged();

}

void ProgressTracker::finishCurrent()
{
    this->save();
    this->m_activeProgressItem = nullptr;
    this->m_skipIdleCheck = true;
}

void ProgressTracker::changeActiveFile(const QUrl& fileUrl)
{
    this->save();
    this->m_activeProgressItem = nullptr;

    this->m_fileUrl = fileUrl;

    emit fileUrlChanged();
}

void ProgressTracker::renameActiveFile(const QUrl& fileUrl)
{
    if (m_activeProgressItem)
        m_activeProgressItem->setFileUrl(fileUrl);

    QString previousUrl = m_fileUrl.toLocalFile();
    m_fileUrl = fileUrl;

    QFile inputFile(progressPath());

    if (!inputFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        emit ErrorManager::instance()->error(tr("Failed to rename file. Couldn't open file."));
        return;
    }

    bool success = persistence::overwrite(inputFile, static_cast<std::function<bool(QTextStream&)>>([&](QTextStream& output) {
        QTextStream input(&inputFile);

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

        return true;
    }));

    if (!success) {
        emit ErrorManager::instance()->error(tr("Failed to rename file."));

        return;
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
        emit ErrorManager::instance()->error(tr("Couldn't open progress.csv."));
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

        const std::function<bool(QTextStream&)> boundFunction = std::bind(&ProgressTracker::write, this, std::placeholders::_1);
        if (persistence::overwrite(inputFile, boundFunction)) {
            m_items_to_save.clear();
            // activeProgressItem must be cleared, else it will never be
            // readded to m_items_to_save
            m_activeProgressItem = nullptr;
        } else {
            emit ErrorManager::instance()->error(tr("Failed to write progress file."));
        }
    }
}

bool ProgressTracker::write(QTextStream& out)
{
    QFile inputFile(progressPath());

    if (!inputFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return false;
    }

    QTextStream input(&inputFile);

    while (!input.atEnd()) {
        const QString line = input.readLine();
        const ProgressItem* matchingItem = nullptr;

        // Try to match items to existing lines and overwrite them
        // if found

        for (ProgressItem* item : m_items_to_save) {
            if (item->fileUrl().isValid() && item->isCsv(line)) {
                matchingItem = item;
                m_items_to_save.removeOne(item);
                break;
            }
        }

        if (matchingItem != nullptr) {
            if (matchingItem->words() != 0) {
                out << matchingItem->toCsv() << '\n';
            }
        } else {
            out << line << '\n';
        }
    }

    // Append the items that could not be matched

    for (ProgressItem* item : m_items_to_save) {
        if (item->fileUrl().isValid() && item->words() != 0) {
            out << item->toCsv() << '\n';
        }
    }

    return true;
}
