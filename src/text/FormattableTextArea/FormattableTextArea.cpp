#include <QFile>
#include <QFileInfo>
#include <QFileSelector>
#include <QDateTime>
#include <QQmlFile>
#include <QQmlFileSelector>
#include <QQuickTextDocument>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QTextDocument>
#include <QTextObject>
#include <QTextFrame>
#include <QTextFragment>
#include <QDebug>

#include "FormattableTextArea.h"
#include "../format.h"
#include "../symbols.h"
#include "../TextHighlighter.h"

namespace {
    constexpr QTextDocument::MarkdownFeatures MARKDOWN_FEATURES = QTextDocument::MarkdownNoHTML;
}

FormattableTextArea::FormattableTextArea(QObject *parent)
    : QObject(parent)
    , m_document(nullptr)
    , m_highlighter(nullptr)
    , m_cursorPosition(-1)
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_characterCount(0)
    , m_wordCount(0)
    , m_paragraphCount(0)
    , m_pageCount(0)
    , m_firstLineIndent(0.0) { }

QQuickTextDocument *FormattableTextArea::document() const
{
    return m_document;
}

void FormattableTextArea::setDocument(QQuickTextDocument *document)
{
    if (document == m_document)
        return;

    if (m_document) {
        m_document->textDocument()->disconnect(this);
    }

    m_document = document;

    // In order to use a custom stylesheet, the text must be set as HTML.
    // Even then, though, it just doesn't work. Reason currently unknown.
    // It may just be broken in the current Qt version.

    // To implement first line indent, line height, and paragraph spacing,
    // check out QAbstractTextDocumentLayout.

    if (m_document) {
        connect(m_document->textDocument(), &QTextDocument::modificationChanged, this, &FormattableTextArea::modifiedChanged);
        connect(m_document->textDocument(), &QTextDocument::contentsChanged, this, &FormattableTextArea::handleTextChange);

        m_highlighter = new TextHighlighter(m_document->textDocument());
    }

    emit documentChanged();
}

void FormattableTextArea::handleTextChange()
{
    // Calling m_highlighter.rehighlight() for some reason sends text change events
    if (!m_highlighter->refreshing()) {
        emit textChanged();
    }

    this->updateCounts();
}

int FormattableTextArea::cursorPosition() const
{
    return m_cursorPosition;
}

void FormattableTextArea::setCursorPosition(int position)
{
    if (position == m_cursorPosition)
        return;

    m_cursorPosition = position;
    reset();
    emit cursorPositionChanged();
}

int FormattableTextArea::selectionStart() const
{
    return m_selectionStart;
}

void FormattableTextArea::setSelectionStart(int position)
{
    if (position == m_selectionStart)
        return;

    m_selectionStart = position;
    emit selectionStartChanged();
}

int FormattableTextArea::selectionEnd() const
{
    return m_selectionEnd;
}

void FormattableTextArea::setSelectionEnd(int position)
{
    if (position == m_selectionEnd)
        return;

    m_selectionEnd = position;
    emit selectionEndChanged();
}

const QTextCharFormat FormattableTextArea::getSelectionFormat() const
{
    return format::getMergedCharFormat(textCursor());
}

void FormattableTextArea::toggleBold()
{
    QTextCharFormat format;
    format.setFontWeight(getSelectionFormat().fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
    mergeFormat(format);
}

void FormattableTextArea::toggleItalics()
{
    QTextCharFormat format;
    format.setFontItalic(!getSelectionFormat().fontItalic());
    mergeFormat(format);
}

void FormattableTextArea::toggleStrikethrough()
{
    QTextCharFormat format;
    format.setFontStrikeOut(!getSelectionFormat().fontStrikeOut());
    mergeFormat(format);
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

void FormattableTextArea::load(const QUrl &fileUrl)
{
    if (fileUrl == m_fileUrl)
        return;

    QQmlEngine *engine = qmlEngine(this);
    if (!engine) {
        qWarning() << "load() called before FormattableTextArea has QQmlEngine";
        return;
    }

    const QUrl path = QQmlFileSelector::get(engine)->selector()->select(fileUrl);
    const QString fileName = QQmlFile::urlToLocalFileOrQrc(path);

    if (!QFile::exists(fileName)) {
        return;
    }

    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        if (QTextDocument *doc = textDocument()) {
            const auto text = codec->toUnicode(data);
            const QString fileType = QFileInfo(file).suffix();

            if (fileType == "md") {
                doc->setMarkdown(text, MARKDOWN_FEATURES);
            } else if (fileType.contains("htm")) {
                doc->setHtml(text);
            } else {
                doc->setPlainText(text);
            }

            doc->setModified(false);
            emit loaded();
        }

        reset();

        m_fileUrl = fileUrl;
        emit fileUrlChanged();
        emit lastModifiedChanged();
    }
}

void FormattableTextArea::saveAs(const QUrl &fileUrl)
{
    QTextDocument *doc = textDocument();

    if (!doc)
        return;

    const QString filePath = fileUrl.toLocalFile();
    const QFileInfo fileInfo = QFileInfo(filePath);
    const QString fileType = fileInfo.suffix();
    const bool isHtml = fileType.contains(QLatin1String("htm"));
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate | (isHtml ? QFile::NotOpen : QFile::Text))) {
        emit error(tr("Cannot save: ") + file.errorString());
        return;
    }

    if (fileType == "md") {
        file.write(doc->toMarkdown(MARKDOWN_FEATURES).toUtf8());
    } else if (fileType.contains("htm")) {
        file.write(doc->toHtml().toUtf8());
    } else {
        file.write(doc->toPlainText().toUtf8());
    }

    file.close();

    this->setModified(false);

    emit lastModifiedChanged();

    if (fileUrl == m_fileUrl)
        return;
}

void FormattableTextArea::reset()
{
}

QTextCursor FormattableTextArea::textCursor() const
{
    QTextDocument *doc = textDocument();
    if (!doc)
        return QTextCursor();

    QTextCursor cursor = QTextCursor(doc);
    if (m_selectionStart != m_selectionEnd) {
        cursor.setPosition(m_selectionStart);
        cursor.setPosition(m_selectionEnd, QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(m_cursorPosition);
    }
    return cursor;
}

QTextDocument *FormattableTextArea::textDocument() const
{
    if (!m_document)
        return nullptr;

    return m_document->textDocument();
}

void FormattableTextArea::mergeFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = textCursor();
    cursor.mergeCharFormat(format);
}

bool FormattableTextArea::modified() const
{
    return m_document && m_document->textDocument()->isModified();
}

void FormattableTextArea::setModified(bool modified)
{
    if (m_document)
        m_document->textDocument()->setModified(modified);
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

    return textDocument()->defaultStyleSheet();
}

void FormattableTextArea::setStyleSheet(const QString& stylesheet)
{
    if (m_document)
        m_document->textDocument()->setDefaultStyleSheet(stylesheet);
}

TextIterator FormattableTextArea::wordIterator() const
{
    TextIterator iterator = TextIterator(this->document()->textDocument()->toPlainText(), TextIterator::IterationType::ByWord);
    iterator.ignoreEnclosedBy(symbols::opening_comment, symbols::closing_comment);

    return iterator;
}
