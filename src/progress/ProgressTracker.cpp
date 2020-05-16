#include <QObject>
#include <QDebug>

#include "ProgressTracker.h"
#include "ProgressItem.h"

ProgressTracker::ProgressTracker(QObject *parent)
    : QObject(parent)
    , m_progressToday(0)
    , m_items()
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
    if (this->m_activeProgressItem != nullptr
            && this->m_activeProgressItem->end().secsTo(QDateTime::currentDateTime()) >= this->m_maximumIdleMinutes * 60) {
        this->m_activeProgressItem = nullptr;
    }

    if (this->m_activeProgressItem == nullptr) {
        // TODO: Retrieve last progress item if it's less than maximumIdleTime past
        this->m_activeProgressItem = new ProgressItem{ this };
        this->m_activeProgressItem->setFileUrl(this->m_fileUrl);
        this->m_activeProgressItem->setStart(QDateTime::currentDateTime());
        this->m_items.append(this->m_activeProgressItem);
        emit itemsChanged();
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

void ProgressTracker::load()
{
    // Consider loading only the items for the current day to save memory.
    // More items can be loaded on-demand in the Progress view.
    // this->m_items.append(new ProgressItem{ this, QUrl(), QDateTime::fromString("2020-05-16T04:05:00", Qt::DateFormat::ISODate), QDateTime::fromString("2020-05-16T04:10:00", Qt::DateFormat::ISODate), 500 });

    const QDate& today = this->getAdjustedDate(QDateTime::currentDateTime());
    const auto end = this->m_items.crend();

    for (auto i = this->m_items.crbegin(); i != end; i++) {
        const QDate& start = this->getAdjustedDate((*i)->start());

        if (start != today) {
            break;
        }

        m_progressToday += (*i)->words();
    }

    if (m_progressToday > 0) {
        emit progressTodayChanged();
    }
}

void ProgressTracker::save()
{

}


const QDate ProgressTracker::getAdjustedDate(const QDateTime& dateTime) {
    return m_dailyReset.isValid() && dateTime.time() < m_dailyReset
        ? dateTime.date().addDays(-1)
        : dateTime.date();
}
