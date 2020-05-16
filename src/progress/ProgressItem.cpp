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
            .arg(m_fileUrl.toLocalFile())
            .arg(m_start.toString(Qt::DateFormat::ISODate))
            .arg(m_end.toString(Qt::DateFormat::ISODate))
            .arg(m_words);
}
