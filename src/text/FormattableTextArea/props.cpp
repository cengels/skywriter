/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///               Contains rudimentary getters and setters.               ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include <QQmlFile>
#include <QFileInfo>
#include <QAbstractTextDocumentLayout>

#include "FormattableTextArea.h"
#include "../../numbers.h"

void FormattableTextArea::setContentY(double contentY)
{
    m_contentY = contentY;
    emit contentYChanged();

    update();
}

int FormattableTextArea::caretPosition() const
{
    return m_textCursor.position();
}

void FormattableTextArea::setCaretPosition(int caretPosition)
{
    int previousPosition = this->caretPosition();

    m_textCursor.setPosition(caretPosition);

    if (this->caretPosition() != previousPosition) {
        updateActive();
    }
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
    return m_document ? m_document->size().height() + m_overflowArea * 2 : -1;
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

QRectF FormattableTextArea::caretRectangle() const
{
    const QTextLine& line = m_textCursor.block().layout()->lineForTextPosition(m_textCursor.positionInBlock());
    const qreal x = line.cursorToX(m_textCursor.positionInBlock()) + m_document->documentLayout()->blockBoundingRect(m_textCursor.block()).topLeft().x();

    return QRectF(x, line.y() + m_textCursor.block().layout()->position().y() + m_overflowArea - m_contentY, 1, line.height());
}

void FormattableTextArea::setReplacement(const QString& target, const QString& replaceWith)
{
    m_replacer.setReplacement(target, replaceWith);
}

void FormattableTextArea::setReplacement(const QChar& target, const QChar& replaceWith)
{
    m_replacer.setReplacement(target, replaceWith);
}

void FormattableTextArea::setSmartReplacement(const QString& target, const QString& replaceWithStart, const QString& replaceWithEnd)
{
    m_replacer.setSmartReplacement(target, replaceWithStart, replaceWithEnd);
}

void FormattableTextArea::setSmartReplacement(const QChar& target, const QChar& replaceWithStart, const QChar& replaceWithEnd)
{
    m_replacer.setSmartReplacement(target, replaceWithStart, replaceWithEnd);
}
