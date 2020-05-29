/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///               Contains rudimentary getters and setters.               ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include <QQmlFile>
#include <QFileInfo>

#include "FormattableTextArea.h"
#include "../../numbers.h"

void FormattableTextArea::setPosition(double position)
{
    m_position = position;
    positionChanged();

    update();
}

int FormattableTextArea::caretPosition() const
{
    return m_textCursor.position();
}

QString FormattableTextArea::fileName() const
{
    const QString filePath = QQmlFile::urlToLocalFileOrQrc(m_fileUrl);
    const QString fileName = QFileInfo(filePath).fileName();
    if (fileName.isEmpty())
        return QStringLiteral("untitled.txt");
    return fileName;
}

QString FormattableTextArea::fileType() const
{
    const QString filePath = QQmlFile::urlToLocalFileOrQrc(m_fileUrl);
    return QFileInfo(filePath).suffix();
}

QDateTime FormattableTextArea::lastModified() const
{
    const QString filePath = QQmlFile::urlToLocalFileOrQrc(m_fileUrl);
    return QFileInfo(filePath).lastModified();
}

QUrl FormattableTextArea::fileUrl() const
{
    return m_fileUrl;
}

QUrl FormattableTextArea::directoryUrl() const
{
    return m_fileUrl.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash);
}

bool FormattableTextArea::modified() const
{
    return m_document && m_document->isModified();
}

void FormattableTextArea::setModified(bool modified)
{
    if (m_document)
        m_document->setModified(modified);
}

bool FormattableTextArea::loading() const
{
    return m_loading;
}

void FormattableTextArea::setFileUrl(const QUrl& url)
{
    bool isSameFolder = m_fileUrl.adjusted(QUrl::RemoveFilename) == url.adjusted(QUrl::RemoveFilename);
    m_fileUrl = url;
    emit fileUrlChanged();

    if (!isSameFolder) {
        emit directoryUrlChanged();
    }
}

QString FormattableTextArea::stylesheet() const
{
    if (!m_document)
        return nullptr;

    return m_document->defaultStyleSheet();
}

void FormattableTextArea::setStyleSheet(const QString& stylesheet)
{
    if (m_document)
        m_document->setDefaultStyleSheet(stylesheet);
}

double FormattableTextArea::contentWidth() const
{
    return m_document ? m_document->size().width() : -1;
}

double FormattableTextArea::contentHeight() const
{
    return m_document ? m_document->size().height() : -1;
}

void FormattableTextArea::selectWord()
{
    m_textCursor.select(QTextCursor::SelectionType::WordUnderCursor);
    emit caretPositionChanged();
    update();
}

void FormattableTextArea::selectParagraph()
{
    m_textCursor.select(QTextCursor::SelectionType::BlockUnderCursor);
    emit caretPositionChanged();
    update();
}
