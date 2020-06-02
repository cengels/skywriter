#include "QQmlFile"

#include "ProgressItem.h"

ProgressItem::ProgressItem(QObject *parent) : QObject(parent),
    m_fileUrl(),
    m_start(),
    m_end(),
    m_words(0)
{ }

ProgressItem::ProgressItem(
        QObject *parent,
        const QUrl& fileUrl,
        const QDateTime& start,
        const QDateTime& end,
        int words
) : QObject(parent),
    m_fileUrl(fileUrl),
    m_start(start),
    m_end(end),
    m_words(words)
{ }

const QUrl ProgressItem::fileUrl() const
{
    return this->m_fileUrl;
}

void ProgressItem::setFileUrl(const QUrl& fileUrl)
{
    this->m_fileUrl = fileUrl;
    emit fileUrlChanged();
}

const QDateTime ProgressItem::start() const
{
    return this->m_start;
}

void ProgressItem::setStart(const QDateTime& dateTime)
{
    this->m_start = dateTime;
    emit startChanged();
}

const QDateTime ProgressItem::end() const
{
    return this->m_end;
}

void ProgressItem::setEnd(const QDateTime& dateTime)
{
    this->m_end = dateTime;
    emit endChanged();
}

int ProgressItem::words() const
{
    return this->m_words;
}

void ProgressItem::setWords(int words)
{
    this->m_words = words;
    emit wordsChanged();
}

QString ProgressItem::toCsv() const {
    return QStringLiteral("%1,%2,%3,%4")
            .arg(QQmlFile::urlToLocalFileOrQrc(m_fileUrl))
            .arg(m_start.toString(Qt::DateFormat::ISODate))
            .arg(m_end.toString(Qt::DateFormat::ISODate))
            .arg(m_words);
}

ProgressItem* ProgressItem::fromCsv(const QString& line) {
    if (line.contains('\n')) {
        qCritical("ProgressItem: fromCsv() argument must be a single line.");

        return new ProgressItem();
    }

    const auto split = line.split(',');

    if (split.length() != 4) {
        qCritical("ProgressItem: Line must have exactly 4 values but had: %d", split.length());

        return new ProgressItem();
    }

    auto* item = new ProgressItem {
        nullptr,
        QUrl::fromLocalFile(split.at(0)),
        QDateTime::fromString(split.at(1), Qt::DateFormat::ISODate),
        QDateTime::fromString(split.at(2), Qt::DateFormat::ISODate),
        split.at(3).toInt()
    };

    return item;
}

bool ProgressItem::isCsv(const QString& line) const
{
    if (line.contains('\n')) {
        qCritical("ProgressItem: isCsv() argument must be a single line.");

        return false;
    }

    const auto split = line.split(',');

    if (split.length() != 4) {
        qCritical("ProgressItem: Line must have exactly 4 values but had: %d", split.length());

        return false;
    }

    return split.at(0) == m_fileUrl.toLocalFile()
        && split.at(1) == m_start.toString(Qt::DateFormat::ISODate);
}
