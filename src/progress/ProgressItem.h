#ifndef PROGRESSITEM_H
#define PROGRESSITEM_H

#include <QObject>
#include <QUrl>
#include <QDateTime>

struct ProgressItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl fileUrl READ fileUrl WRITE setFileUrl NOTIFY fileUrlChanged);
    Q_PROPERTY(QDateTime start READ start WRITE setStart NOTIFY startChanged);
    Q_PROPERTY(QDateTime end READ end WRITE setEnd NOTIFY endChanged);
    Q_PROPERTY(int words READ words WRITE setWords NOTIFY wordsChanged);

    public:
        explicit ProgressItem(QObject *parent = nullptr);
        explicit ProgressItem(
                QObject *parent,
                const QUrl& fileUrl,
                const QDateTime& start,
                const QDateTime& end,
                int words
        );

        const QUrl fileUrl() const;
        void setFileUrl(const QUrl& fileUrl);

        const QDateTime start() const;
        void setStart(const QDateTime& dateTime);

        const QDateTime end() const;
        void setEnd(const QDateTime& dateTime);

        int words() const;
        void setWords(int words);

        QString toCsv() const;
        static ProgressItem* fromCsv(const QString& line);
        //! Checks if the given CSV line corresponds with this ProgressItem.
        //! This method does not instantiate a new ProgressItem and should
        //! therefore be preferred in most instances.
        bool isCsv(const QString& line) const;

    Q_SIGNALS:
        void fileUrlChanged();
        void startChanged();
        void endChanged();
        void wordsChanged();

    private:
        QUrl m_fileUrl;
        QDateTime m_start;
        QDateTime m_end;
        int m_words;

};

#endif // PROGRESSITEM_H
