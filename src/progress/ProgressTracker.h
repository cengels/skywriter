#ifndef PROGRESSTRACKER_H
#define PROGRESSTRACKER_H

#include <QObject>
#include <QString>
#include <QUrl>

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
    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged);
    Q_PROPERTY(int maximumIdleMinutes READ maximumIdleMinutes WRITE setMaximumIdleMinutes NOTIFY maximumIdleMinutesChanged);
    Q_PROPERTY(int autosaveMinutes READ autosaveMinutes WRITE setAutosaveMinutes NOTIFY autosaveMinutesChanged);

    public:
        explicit ProgressTracker(QObject *parent = nullptr);

        int progressToday() const;

        int maximumIdleMinutes() const;
        void setMaximumIdleMinutes(const int minutes);

        int autosaveMinutes() const;
        void setAutosaveMinutes(const int minutes);

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

    Q_SIGNALS:
        void progressTodayChanged();
        void maximumIdleMinutesChanged();
        void autosaveMinutesChanged();
        void fileUrlChanged();
        void activeFileChanged();

    private:
        void load();
        void save();

        int m_progressToday;
        int m_maximumIdleMinutes;
        int m_autosaveMinutes;
        QUrl m_fileUrl;
};

#endif // PROGRESSTRACKER_H
