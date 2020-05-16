#include <QObject>
#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QGuiApplication>

#include "ProgressTracker.h"
#include "ProgressItem.h"
#include "../persistence.h"

namespace {
    QString m_progressPath = QString();
    QString progressPath()
    {
        if (m_progressPath.isNull()) {
            m_progressPath = persistence::documentsPath() + "/progress2.csv";
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
    , m_maximumIdleMinutes(0)
    , m_autosaveMinutes(0)
    , m_dailyReset()
    , m_fileUrl()
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
        if (!skipIdleCheck && !this->m_items.isEmpty() && this->m_items.constLast()->end().secsTo(now) <= this->m_maximumIdleMinutes * 60) {
            this->m_activeProgressItem = this->m_items.last();
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
    this->m_fileUrl = fileUrl;

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
            ProgressItem* item = ProgressItem::fromCsv(line);
            const QDate& date = getAdjustedDate(item->start(), adjustBy);

            if (date.year() == year && (month == 0 || date.month() == month)) {
                item->setParent(parent);
                list.append(item);
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

    if (m_progressToday > 0) {
        emit progressTodayChanged();
    }
}

void ProgressTracker::save()
{
    if (!m_items_to_save.isEmpty()) {
        QFile file(progressPath());
        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);

            foreach (const ProgressItem* item, m_items_to_save) {
                if (item->words() != 0) {
                    out << item->toCsv() << '\n';
                }
            }

            file.close();
            m_items_to_save.clear();
        } else {
            qCritical("Couldn't open progress.csv.");
        }
    }
}
