/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///               Contains rudimentary getters and setters.               ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////

#include <QQmlFile>
#include <QFileInfo>
#include <QAbstractTextDocumentLayout>
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeData>

#include "FormattableTextArea.h"
#include "../../numbers.h"

QTextDocument* FormattableTextArea::document() const
{
    return m_document;
}

const QVector<DocumentSegment*>& FormattableTextArea::documentStructure() const
{
    return m_documentStructure;
}

double FormattableTextArea::contentY() const
{
    return m_contentY;
}

void FormattableTextArea::setContentY(double contentY)
{
    m_contentY = contentY;
    emit contentYChanged();

    update();
}

double FormattableTextArea::overflowArea() const
{
    return m_overflowArea;
}

void FormattableTextArea::setOverflowArea(double overflowArea)
{
    m_overflowArea = overflowArea;
    emit overflowAreaChanged();

    update();
}

int FormattableTextArea::caretPosition() const
{
    return m_textCursor.position();
}

void FormattableTextArea::setCaretPosition(int caretPosition)
{
    int previousPosition = this->caretPosition();

    if (caretPosition < 0) {
        m_textCursor.movePosition(QTextCursor::MoveOperation::Start);
    } else if (caretPosition > m_document->characterCount() - 1) {
        m_textCursor.movePosition(QTextCursor::MoveOperation::End);
    } else {
        m_textCursor.setPosition(caretPosition);
    }

    if (this->caretPosition() != previousPosition) {
        updateActive();
        emit caretPositionChanged();
    }
}

QString FormattableTextArea::fileName() const
{
    const QString filePath = QQmlFile::urlToLocalFileOrQrc(m_fileUrl);
    const QString fileName = QFileInfo(filePath).fileName();
    if (fileName.isEmpty())
        return QStringLiteral("untitled");
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

bool FormattableTextArea::canUndo() const
{
    return m_document->isUndoAvailable();
}

bool FormattableTextArea::canRedo() const
{
    return m_document->isRedoAvailable();
}

bool FormattableTextArea::canPaste() const
{
    return QGuiApplication::clipboard()->mimeData()
        && (QGuiApplication::clipboard()->mimeData()->hasText()
        || QGuiApplication::clipboard()->mimeData()->hasHtml());
}

QString FormattableTextArea::selectedText() const
{
    return m_textCursor.selectedText();
}

const QVector<Range<int>>& FormattableTextArea::searchResults() const
{
    return m_highlighter->findRanges();
}

int FormattableTextArea::searchResultCount() const
{
    return m_highlighter->findRanges().size();
}

QUrl FormattableTextArea::fileUrl() const
{
    return m_fileUrl;
}

QUrl FormattableTextArea::directoryUrl() const
{
    return m_fileUrl.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash);
}

bool FormattableTextArea::fileExists() const
{
    const QString filePath = QQmlFile::urlToLocalFileOrQrc(m_fileUrl);
    return m_document && !m_fileUrl.isEmpty() && !QFileInfo(filePath).fileName().isEmpty();
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
    const bool isSameFolder = m_fileUrl.adjusted(QUrl::RemoveFilename) == url.adjusted(QUrl::RemoveFilename);
    const bool didExist = fileExists();
    m_fileUrl = url;
    emit fileUrlChanged();

    if (!didExist && fileExists()) {
        emit fileExistsChanged();
    }

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
