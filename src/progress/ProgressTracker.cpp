#include <QObject>
#include <QDebug>

#include "ProgressTracker.h"

ProgressTracker::ProgressTracker(QObject *parent)
    : QObject(parent)
    , m_progressToday(0)
    , m_maximumIdleMinutes(0)
    , m_autosaveMinutes(0)
    , m_fileUrl(QUrl())
{
    this->load();
}

int ProgressTracker::progressToday() const
{
    return this->m_progressToday;
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

QUrl ProgressTracker::fileUrl() const
{
    return this->m_fileUrl;
}

void ProgressTracker::addProgress(const int words)
{
    qDebug() << "Adding " << words << " words";
}

void ProgressTracker::changeActiveFile(const QUrl& fileUrl)
{
    qDebug() << "Changing active file to " << fileUrl.url();
}

void ProgressTracker::renameActiveFile(const QUrl& fileUrl)
{
    qDebug() << "Renaming active file to " << fileUrl.url();
}

void ProgressTracker::load()
{

}

void ProgressTracker::save()
{

}
