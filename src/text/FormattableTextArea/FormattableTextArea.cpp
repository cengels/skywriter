#include <QCursor>
#include <QAbstractTextDocumentLayout>
#include <QQmlFile>
#include <QTextCodec>
#include <QFileInfo>

#include "FormattableTextArea.h"
#include "../symbols.h"
#include "../selection.h"
#include "../../persistence.h"
#include "../../ErrorManager.h"
#include "../UserData.h"

FormattableTextArea::FormattableTextArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_document(nullptr)
    , m_documentStructure(QVector<DocumentSegment*>())
    , m_currentDocumentSegment(nullptr)
    , m_highlighter(nullptr)
    , m_replacer(StringReplacer())
    , m_textCursor(QTextCursor())
    , m_contentY(0.0)
    , m_overflowArea(0.0)
    , m_fileUrl()
    , m_loading(false)
    , m_characterCount(0)
    , m_selectedCharacterCount(0)
    , m_wordCount(0)
    , m_selectedWordCount(0)
    , m_paragraphCount(0)
    , m_selectedParagraphCount(0)
    , m_pageCount(0)
    , m_selectedPageCount(0)
    , m_searchString()
    , m_searchFlags()
    , m_searchResults(QVector<Range<int>>())
    , m_underline(false)
    , m_sceneBreak()
    , m_caretTimer(this)
    , m_blinking(false)
    , m_activeWordCounters(0)
    , m_lastMouseUpEvent(QMouseEvent(QMouseEvent::None, QPointF(), Qt::NoButton, 0, 0))
    , m_lastMouseDownEvent(QMouseEvent(QMouseEvent::None, QPointF(), Qt::NoButton, 0, 0))
    , m_lastCaretPosition(0)
    , m_selectionMode(SelectionMode::NoSelection)
{
    // Ideally, we would retrieve the system-wide caret blink rate here.
    // But I'm not sure how to do that/if that's even possible out of Qt.
    m_caretTimer.setInterval(530);
    m_caretTimer.callOnTimeout(this, [&]() {
        m_blinking = !m_blinking;
        update();
    });

    setFiltersChildMouseEvents(true);
    setAcceptedMouseButtons(Qt::MouseButton::AllButtons);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);

    QCursor cursor;
    cursor.setShape(Qt::CursorShape::IBeamCursor);
    setCursor(cursor);
    setAntialiasing(true);
    setFlag(QQuickItem::ItemHasContents, true);

    connect(this, &FormattableTextArea::widthChanged, this, [this]() {
        if (this->width() > 0) {
            this->m_document->setTextWidth(this->width());
        }
    });

    connect(ThemeManager::instance(), &ThemeManager::activeThemeChanged, this, &FormattableTextArea::updateStyling);
    connect(this, &FormattableTextArea::overflowAreaChanged, this, [&] {
        FormattableTextArea::update();

        emit contentHeightChanged();
    });

    connect(this, &FormattableTextArea::selectedTextChanged, this, &FormattableTextArea::updateSelectedCounts);
    connect(this, &FormattableTextArea::caretPositionChanged, this, [&] {
        const int characterCount = m_document->characterCount();
        DocumentSegment* oldSegment = findDocumentSegment(m_lastCaretPosition >= characterCount ? characterCount - 1 : m_lastCaretPosition);
        DocumentSegment* newSegment = findDocumentSegment(m_textCursor.position());

        if (oldSegment != newSegment) {
            m_currentDocumentSegment = newSegment;
            emit currentDocumentSegmentChanged();
        }

        m_lastCaretPosition = m_textCursor.position();
    });

    newDocument();
    connectDocument();
}

QTextDocument* FormattableTextArea::newDocument()
{
    if (m_document) {
        m_document->disconnect(this);
    }

    m_document = new QTextDocument(this);

    this->updateStyling();
    m_textCursor = QTextCursor(m_document);

    return m_document;
}

void FormattableTextArea::connectDocument()
{
    if (m_document) {
        connect(m_document, &QTextDocument::modificationChanged, this, &FormattableTextArea::modifiedChanged);
        connect(m_document, &QTextDocument::contentsChange, this, &FormattableTextArea::handleTextChange);
        connect(m_document, &QTextDocument::undoAvailable, this, &FormattableTextArea::canUndoChanged);
        connect(m_document, &QTextDocument::redoAvailable, this, &FormattableTextArea::canRedoChanged);
        connect(m_document->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, [&] {
            emit contentHeightChanged();
            emit contentWidthChanged();
        });
        connect(m_document->documentLayout(), &QAbstractTextDocumentLayout::update, this, &FormattableTextArea::update);

        if (m_highlighter) {
            m_highlighter->setDocument(m_document);
        } else {
            m_highlighter = new TextHighlighter(m_document);
            m_highlighter->setSceneBreak(m_sceneBreak);
            m_highlighter->setFindRanges(&m_searchResults);
            connect(this, &FormattableTextArea::sceneBreakChanged, m_highlighter, &TextHighlighter::setSceneBreak);
        }
    }

    // must be called before emitting caretPositionChanged()
    // so that the current document segment can be found
    refreshDocumentStructure();

    emit caretPositionChanged();
    emit selectedTextChanged();

    if (canPaste()) {
        emit canPasteChanged();
    }

    clearUndoStack();

    emit documentChanged();
    emit modifiedChanged();
    emit lastModifiedChanged();
    update();
}

void FormattableTextArea::handleTextChange(const int position, const int removed, const int added)
{
    // TODO: Fix this. For some reason, using edit blocks here doesn't work.
    // The undo manager just stops working for some reason and you can't undo
    // anything anymore.
    if (format::isSceneBreak(m_textCursor) && m_textCursor.block().text() != m_sceneBreak) {
        m_textCursor.joinPreviousEditBlock();
        m_textCursor.setBlockFormat(ThemeManager::instance()->activeTheme()->blockFormat());
        m_textCursor.endEditBlock();
    } else if (!format::isSceneBreak(m_textCursor) && m_textCursor.block().text() == m_sceneBreak) {
        m_textCursor.joinPreviousEditBlock();
        format::insertSceneBreak(m_textCursor, m_sceneBreak, true);
        m_textCursor.endEditBlock();
    }

    updateCounts();
    updateFindRanges();

    if (added != 0 || removed != 0) {
        updateDocumentStructure(position, added, removed);
        emit textChanged(position, added, removed);
    }
}

void FormattableTextArea::updateActive()
{
    m_blinking = false;
    m_caretTimer.start();

    update();
}

const QTextCharFormat FormattableTextArea::getSelectionFormat() const
{
    return format::getMergedCharFormat(m_textCursor);
}

void FormattableTextArea::clearReplacements()
{
    m_replacer.clear();
}

void FormattableTextArea::load(const QUrl &fileUrl)
{
    if (fileUrl == m_fileUrl)
        return;

    const QString fileName = QQmlFile::urlToLocalFileOrQrc(fileUrl);

    if (!QFile::exists(fileName)) {
        return;
    }

    m_loading = true;
    emit loadingChanged();

    clearMatches();

    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        newDocument();
        const auto text = codec->toUnicode(data);
        const QString fileType = QFileInfo(file).suffix();

        if (fileType == "md") {
            MarkdownParser(m_document, m_sceneBreak).parse(text);
        } else {
            m_document->setPlainText(text);
        }

        m_document->setModified(false);
        connectDocument();
        emit loaded();

        setFileUrl(fileUrl);
        emit lastModifiedChanged();
    }

    m_loading = false;

    if (!m_loading)
        emit loadingChanged();
}

void FormattableTextArea::saveAs(const QUrl &fileUrl, bool keepBackup)
{
    if (!m_document)
        return;

    const QString filePath = QQmlFile::urlToLocalFileOrQrc(fileUrl);
    const QFileInfo fileInfo = QFileInfo(filePath);
    const QString fileType = fileInfo.suffix();

    QFile file(filePath);

    bool success = persistence::overwrite(file, static_cast<std::function<bool(QTextStream&)>>([&](QTextStream& stream)
    {
        if (fileType == "md") {
            MarkdownParser(m_document, m_sceneBreak).write(stream);
        } else if (fileType.contains("htm")) {
            stream << m_document->toHtml().toUtf8();
        } else {
            stream << m_document->toPlainText().toUtf8();
        }

        return true;
    }), keepBackup);

    if (!success) {
        emit ErrorManager::instance()->error(tr("Cannot save: ") + file.errorString());
    } else {
        this->setModified(false);

        emit lastModifiedChanged();

        if (fileUrl != m_fileUrl)
            setFileUrl(fileUrl);
    }
}

void FormattableTextArea::backup()
{
    if (!m_document || !m_fileUrl.isValid())
        return;

    const QFileInfo fileInfo = QFileInfo(QQmlFile::urlToLocalFileOrQrc(m_fileUrl));
    const QString fileType = fileInfo.suffix();
    QFile backupFile(QQmlFile::urlToLocalFileOrQrc(m_fileUrl) + ".bak");

    bool success = persistence::overwrite(backupFile, static_cast<std::function<bool(QTextStream&)>>([&](QTextStream& stream)
    {
        if (fileType == "md") {
            MarkdownParser(m_document, m_sceneBreak).write(stream);
        } else if (fileType.contains("htm")) {
            stream << m_document->toHtml().toUtf8();
        } else {
            stream << m_document->toPlainText().toUtf8();
        }

        return true;
    }), false);

    if (!success) {
        emit ErrorManager::instance()->error(tr("Cannot backup: ") + backupFile.errorString());
    }
}

bool FormattableTextArea::rename(const QUrl& newName)
{
    if (!fileExists()) {
        return false;
    }

    QFile file(QQmlFile::urlToLocalFileOrQrc(m_fileUrl));

    if (file.exists() && file.rename(QQmlFile::urlToLocalFileOrQrc(newName))) {
        setFileUrl(newName);

        return true;
    }

    return false;
}

void FormattableTextArea::reset()
{
    newDocument();
    connectDocument();
    m_characterCount = 0;
    m_wordCount = 0;
    m_paragraphCount = 0;
    m_pageCount = 0;
    clearMatches();
    emit characterCountChanged();
    emit wordCountChanged(false);
    emit paragraphCountChanged();
    emit pageCountChanged();
    setFileUrl(QUrl());
}

void FormattableTextArea::moveCursor(QTextCursor::MoveOperation op, QTextCursor::MoveMode mode, int by)
{
    const bool hadSelection = m_textCursor.hasSelection();

    switch (op) {
        case QTextCursor::NextWord:
            selection::moveToNextWord(m_textCursor, mode);
            break;
        case QTextCursor::PreviousWord:
            selection::moveToPreviousWord(m_textCursor, mode);
            break;
        default: m_textCursor.movePosition(op, mode, by);
    }

    updateActive();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }
}

void FormattableTextArea::clearUndoStack()
{
    m_document->setUndoRedoEnabled(false);
    m_document->setUndoRedoEnabled(true);
}

void FormattableTextArea::mergeFormat(const QTextCharFormat &format)
{
    m_textCursor.mergeCharFormat(format);
}
