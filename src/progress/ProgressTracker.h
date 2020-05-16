#ifndef PROGRESSTRACKER_H
#define PROGRESSTRACKER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QList>
#include <QTime>

#include "ProgressItem.h"

/*!
    Tracks and retrieves progress made on a text document and stores that
    progress on the file system. Each progress item contains the file name
    of the associated document, the start and end times of writing, and the
    progress (in words) made in that time.

    Progress items are automatically closed and saved when maximumIdleMinutes
    have passed since the last progress was tracked (i.e. the last word change).
    A maximumIdleMinutes value of 0 means that items are never closed after
    a specific period of time and are only saved when the application is closed.
*/
class ProgressTracker : public QObject {
    Q_OBJECT

    Q_PROPERTY(int progressToday READ progressToday NOTIFY progressTodayChanged);
    Q_PROPERTY(const QList<ProgressItem*> items READ items NOTIFY itemsChanged);
    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged);
    Q_PROPERTY(int maximumIdleMinutes READ maximumIdleMinutes WRITE setMaximumIdleMinutes NOTIFY maximumIdleMinutesChanged);
    Q_PROPERTY(int autosaveMinutes READ autosaveMinutes WRITE setAutosaveMinutes NOTIFY autosaveMinutesChanged);
    Q_PROPERTY(QTime dailyReset READ dailyReset WRITE setDailyReset NOTIFY dailyResetChanged);

    public:
        explicit ProgressTracker(QObject *parent = nullptr);

        int progressToday() const;
        QList<ProgressItem*> items() const;

        int maximumIdleMinutes() const;
        void setMaximumIdleMinutes(const int minutes);

        int autosaveMinutes() const;
        void setAutosaveMinutes(const int minutes);

        QTime dailyReset() const;
        void setDailyReset(const QTime& time);

        QUrl fileUrl() const;

    public Q_SLOTS:
        //! Adds the specified number of words for the currently active file.
        void addProgress(const int words);
        //! Sets a new file. Note that this closes any active progress items.
        void changeActiveFile(const QUrl& fileUrl);
        //! Renames the active file. This updates the file name in any
        //! currently active progress items in addition to renaming and resaving
        //! all progress items associated with this file name.
        void renameActiveFile(const QUrl& fileUrl);
        //! Loads all available progress items from the file system.
        void load();

    Q_SIGNALS:
        void progressTodayChanged();
        void itemsChanged();
        void maximumIdleMinutesChanged();
        void autosaveMinutesChanged();
        void dailyResetChanged();
        void fileUrlChanged();
        void activeFileChanged();

    private:
        void save();
        //! Gets the date component of the specified QDateTime. If the time
        //! is before the specified dailyReset, the previous day is returned.
        const QDate getAdjustedDate(const QDateTime& dateTime);

        int m_progressToday;
        QList<ProgressItem*> m_items;
        ProgressItem* m_activeProgressItem;
        int m_maximumIdleMinutes;
        int m_autosaveMinutes;
        QTime m_dailyReset;
        QUrl m_fileUrl;
};

#endif // PROGRESSTRACKER_H