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
#include <QCursor>
#include <QPainter>
#include <QDebug>
#include <QAbstractTextDocumentLayout>
#include <QTextDocumentFragment>
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeData>

#include "FormattableTextArea.h"
#include "../format.h"
#include "../TextHighlighter.h"
#include "../../theming/ThemeManager.h"
#include "../../ErrorManager.h"
#include "../../persistence.h"
#include "../symbols.h"

FormattableTextArea::FormattableTextArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_document(nullptr)
    , m_documentStructure(QVector<DocumentSegment*>())
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
    , m_firstLineIndent(0.0)
    , m_underline(false)
    , m_sceneBreak()
    , m_caretTimer(this)
    , m_blinking(false)
    , m_lastMouseUpEvent(QMouseEvent(QMouseEvent::None, QPointF(), Qt::NoButton, 0, 0))
    , m_lastMouseDownEvent(QMouseEvent(QMouseEvent::None, QPointF(), Qt::NoButton, 0, 0))
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
            update();
        }
    });

    connect(ThemeManager::instance(), &ThemeManager::activeThemeChanged, this, &FormattableTextArea::updateStyling);
    connect(this, &FormattableTextArea::overflowAreaChanged, this, [&] {
        FormattableTextArea::update();

        emit contentHeightChanged();
    });

    connect(this, &FormattableTextArea::selectedTextChanged, this, &FormattableTextArea::updateSelectedCounts);

    connect(this, &FormattableTextArea::textInserted, this, [&] (const int at, const QString text) {
        DocumentSegment* targetSegment = nullptr;

        for (DocumentSegment* segment : m_documentStructure) {
            if (targetSegment) {
                segment->setPosition(segment->position() + text.length());
                continue;
            }

            const DocumentSegment* next = segment->next();

            if (next == nullptr || (at >= segment->position() && at < next->position())) {
                targetSegment = segment;
            }
        }

        emit targetSegment->textChanged();

        if (at == 0
                || symbols::isWordSeparator(m_document->toPlainText().at(at - 1))
                || (text.size() > 1 && symbols::containsWordSeparator(text))) {
            targetSegment->updateWords();
        }
    });

    connect(this, &FormattableTextArea::textRemoved, this, [&] (const int at, const QString text) {
        int textEnd = at + text.length();
        int length = m_documentStructure.length();
        DocumentSegment* targetSegment = nullptr;

        for (int i = 0; i < length; i++) {
            DocumentSegment* segment = m_documentStructure.at(i);

            if (targetSegment) {
                segment->setPosition(segment->position() - text.length());
                continue;
            }

            if (i == length - 1) {
                targetSegment = segment;
                break;
            }

            DocumentSegment* next = m_documentStructure.at(i + 1);

            if (at >= segment->position()) {
                if (textEnd < next->position()) {
                    targetSegment = segment;
                } else if (targetSegment) {
                    // Text was removed from more than one DocumentSegment.
                    // This means that a DocumentSegment boundary was removed,
                    // i.e. the number of DocumentSegments is not the same
                    // as before. Must reinstantiate the entire structure.

                    updateDocumentStructure();
                    return;
                }
            }
        }

        emit targetSegment->textChanged();

        if (symbols::isWordSeparator(m_document->toPlainText().at(at - 1)) || symbols::containsWordSeparator(text)) {
            targetSegment->updateWords();
        }
    });

    newDocument();
}

void FormattableTextArea::newDocument(QTextDocument* document)
{
    if (m_document) {
        m_document->disconnect(this);
    }

    if (document && document->parent() == nullptr) {
        document->setParent(this);
    }

    m_document = document;

    // In order to use a custom stylesheet, the text must be set as HTML.
    // Even then, though, it just doesn't work. Reason currently unknown.
    // It may just be broken in the current Qt version.

    // To implement first line indent, line height, and paragraph spacing,
    // check out QAbstractTextDocumentLayout.

    if (m_document) {
        connect(m_document, &QTextDocument::modificationChanged, this, &FormattableTextArea::modifiedChanged);
        connect(m_document, &QTextDocument::contentsChanged, this, &FormattableTextArea::handleTextChange);
        connect(m_document, &QTextDocument::undoAvailable, this, &FormattableTextArea::canUndoChanged);
        connect(m_document, &QTextDocument::redoAvailable, this, &FormattableTextArea::canRedoChanged);

        if (m_highlighter) {
            m_highlighter->setDocument(m_document);
        } else {
            m_highlighter = new TextHighlighter(m_document);
            m_highlighter->setSceneBreak(m_sceneBreak);
            connect(this, &FormattableTextArea::sceneBreakChanged, m_highlighter, &TextHighlighter::setSceneBreak);
        }

        this->updateStyling();
        m_textCursor = QTextCursor(m_document);
        emit caretPositionChanged();
        emit selectedTextChanged();

        if (canPaste()) {
            emit canPasteChanged();
        }
    }

    clearUndoStack();
    updateDocumentStructure();

    emit documentChanged();
    emit modifiedChanged();
    emit lastModifiedChanged();
    update();
}

void FormattableTextArea::handleTextChange()
{
    emit contentHeightChanged();

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

    this->updateCounts();
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

void FormattableTextArea::applyHeading(int level)
{
    if (level < 0 || level > 6) {
        emit ErrorManager::instance()->warning(tr("Only headings between 0 and 6 are permitted."));
        return;
    }

    if (level == 0) {
        const Theme* theme = ThemeManager::instance()->activeTheme();
        m_textCursor.setBlockFormat(theme->blockFormat());
        format::mergeBlockCharFormat(m_textCursor, theme->charFormat());
    } else {
        const HeadingFormat& format = ThemeManager::instance()->activeTheme()->headingFormat(level);
        m_textCursor.setBlockFormat(format.blockFormat());
        m_highlighter->refresh();
    }

    updateActive();
}

void FormattableTextArea::insertSceneBreak()
{
    if (m_sceneBreak.isEmpty()) {
        return;
    }

    m_textCursor.beginEditBlock();

    format::insertSceneBreak(m_textCursor, m_sceneBreak);

    m_textCursor.endEditBlock();
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

    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        if (QTextDocument *doc = new QTextDocument(this)) {
            const auto text = codec->toUnicode(data);
            const QString fileType = QFileInfo(file).suffix();

            if (fileType == "md") {
                MarkdownParser(doc, m_sceneBreak).parse(text);
            } else {
                doc->setPlainText(text);
            }

            doc->setModified(false);
            newDocument(doc);
            emit loaded();
        }

        setFileUrl(fileUrl);
        emit lastModifiedChanged();
    }

    m_loading = false;

    if (!m_loading)
        emit loadingChanged();
}

void FormattableTextArea::saveAs(const QUrl &fileUrl)
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
    }));

    if (!success) {
        emit ErrorManager::instance()->error(tr("Cannot save: ") + file.errorString());
    } else {
        this->setModified(false);

        emit lastModifiedChanged();

        if (fileUrl != m_fileUrl)
            setFileUrl(fileUrl);
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
    newDocument(new QTextDocument());
    m_characterCount = 0;
    m_wordCount = 0;
    m_paragraphCount = 0;
    m_pageCount = 0;
    emit characterCountChanged();
    emit wordCountChanged(false);
    emit paragraphCountChanged();
    emit pageCountChanged();
    setFileUrl(QUrl());
}

void FormattableTextArea::copy()
{
    if (m_textCursor.hasSelection()) {
        bool couldPaste = canPaste();

        QClipboard* clipboard = QGuiApplication::clipboard();
        QMimeData* mimeData = new QMimeData();
        mimeData->setText(m_textCursor.selection().toPlainText());
        mimeData->setHtml(m_textCursor.selection().toHtml("utf-8"));
        clipboard->setMimeData(mimeData);

        if (!couldPaste) {
            emit canPasteChanged();
        }
    }
}

void FormattableTextArea::paste()
{
    if (!canPaste()) {
        return;
    }

    bool hadSelection = m_textCursor.hasSelection();
    int previousPosition = m_textCursor.selectionStart();
    QString insertedString;
    const QMimeData* mimeData = QGuiApplication::clipboard()->mimeData();

    if (mimeData->hasHtml()) {
        m_textCursor.insertHtml(mimeData->html());
        int newPosition = m_textCursor.position();
        m_textCursor.setPosition(previousPosition);
        m_textCursor.setPosition(newPosition, QTextCursor::KeepAnchor);
        insertedString = m_textCursor.selectedText();
        format::normalize(m_textCursor, ThemeManager::instance()->activeTheme(), m_sceneBreak);
        m_textCursor.clearSelection();
    } else if (mimeData->hasText()) {
        insertedString = mimeData->text();
        m_textCursor.insertText(mimeData->text());
    }

    updateActive();
    emit caretPositionChanged();
    emit textChanged();
    emit textInserted(previousPosition, insertedString);

    if (hadSelection) {
        emit selectedTextChanged();
    }
}

void FormattableTextArea::remove()
{
    if (m_textCursor.hasSelection()) {
        int position = m_textCursor.selectionStart();
        QString text = m_textCursor.selectedText();

        if (text.size() == 1) {
            // This distinction is made because removeSelectedText() immediately
            // adds a new undo state, which is undesired for small changes
            // that add or remove only one character.
            m_textCursor.clearSelection();
            m_textCursor.setPosition(position);
            m_textCursor.deleteChar();
        } else {
            m_textCursor.removeSelectedText();
        }

        emit selectedTextChanged();
        emit textChanged();
        emit textRemoved(position, text);
        emit caretPositionChanged();
    }
}

void FormattableTextArea::cut()
{
    copy();
    remove();
}

void FormattableTextArea::undo()
{
    bool hadSelection = m_textCursor.hasSelection();
    m_document->undo(&m_textCursor);
    updateActive();
    emit textChanged();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }

    updateDocumentStructure();
}

void FormattableTextArea::redo()
{
    bool hadSelection = m_textCursor.hasSelection();
    m_document->redo(&m_textCursor);
    updateActive();
    emit textChanged();
    emit caretPositionChanged();

    if (hadSelection) {
        emit selectedTextChanged();
    }

    updateDocumentStructure();
}

void FormattableTextArea::updateDocumentStructure()
{
    for (DocumentSegment* segment : m_documentStructure) {
        delete segment;
    }

    m_documentStructure.clear();

    if (!m_document) {
        emit documentStructureChanged();

        return;
    }

    m_documentStructure.append(new DocumentSegment(0, 1, this));

    QTextBlock previous = QTextBlock();
    QTextBlock previousHeading = QTextBlock();
    QTextBlock block = m_document->firstBlock();

    while (block.isValid()) {
        bool isHeading = block.blockFormat().headingLevel() > 0;
        bool isSubheading = previous.blockFormat().headingLevel() == block.blockFormat().headingLevel() - 1
                            // uneven headings are considered subheadings
                            && block.blockFormat().headingLevel() % 2 == 0;

        if (isHeading && !isSubheading) {
            if (previous.isValid()) {
                int depth;
                DocumentSegment* const previousSegment = m_documentStructure.last();

                if (!previousHeading.isValid() || previousHeading.blockFormat().headingLevel() == block.blockFormat().headingLevel()) {
                    depth = previousSegment->depth();
                } else {
                    depth = previousSegment->depth() + 1;
                }

                m_documentStructure.append(new DocumentSegment(block.position(), depth, this));

                if (previousSegment) {
                    emit previousSegment->textChanged();
                    previousSegment->updateWords();
                }
            }

            previousHeading = block;
        }

        previous = block;
        block = block.next();
    }

    emit m_documentStructure.last()->textChanged();
    m_documentStructure.last()->updateWords();
    emit documentStructureChanged();
}

void FormattableTextArea::moveCursor(QTextCursor::MoveOperation op, QTextCursor::MoveMode mode, int by)
{
    bool hadSelection = m_textCursor.hasSelection();

    m_textCursor.movePosition(op, mode, by);
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
