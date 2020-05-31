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

namespace {
    constexpr QTextDocument::MarkdownFeatures MARKDOWN_FEATURES = QTextDocument::MarkdownNoHTML;
}

FormattableTextArea::FormattableTextArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_document(nullptr)
    , m_highlighter(nullptr)
    , m_textCursor(QTextCursor())
    , m_contentY(0.0)
    , m_fileUrl()
    , m_loading(false)
    , m_characterCount(0)
    , m_wordCount(0)
    , m_paragraphCount(0)
    , m_pageCount(0)
    , m_firstLineIndent(0.0)
    , m_underline(false)
{
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

//    connect(this, &FormattableTextArea::heightChanged, this, [this]() {
//        if (this->height() > 0) {
//            update();
//        }
//    });

    connect(ThemeManager::instance(), &ThemeManager::activeThemeChanged, this, &FormattableTextArea::updateStyling);

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

        if (m_highlighter) {
            m_highlighter->setDocument(m_document);
        } else {
            m_highlighter = new TextHighlighter(m_document);
        }

        this->updateStyling();
        this->updateWordCount();
        m_textCursor = QTextCursor(m_document);
    }

    emit documentChanged();
    update();
}

void FormattableTextArea::handleTextChange()
{
    // Calling m_highlighter.rehighlight() for some reason sends text change events
    if (!m_highlighter->refreshing()) {
        emit textChanged();
    }

    emit contentHeightChanged();

    this->updateCounts();
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

void FormattableTextArea::load(const QUrl &fileUrl)
{
    if (fileUrl == m_fileUrl)
        return;

    const QString fileName = QQmlFile::urlToLocalFileOrQrc(fileUrl);

    if (!QFile::exists(fileName)) {
        return;
    }

    m_loading = true;
    loadingChanged();

    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        if (QTextDocument *doc = new QTextDocument(this)) {
            const auto text = codec->toUnicode(data);
            const QString fileType = QFileInfo(file).suffix();

            if (fileType == "md") {
                MarkdownParser(doc).parse(text);
            } else {
                doc->setPlainText(text);
            }

            doc->setModified(false);
            newDocument(doc);
            emit loaded();
        }

        m_fileUrl = fileUrl;
        emit fileUrlChanged();
        emit lastModifiedChanged();
    }

    m_loading = false;
    emit loadingChanged();
}

void FormattableTextArea::saveAs(const QUrl &fileUrl)
{
    if (!m_document)
        return;

    const QString filePath = fileUrl.toLocalFile();
    const QFileInfo fileInfo = QFileInfo(filePath);
    const QString fileType = fileInfo.suffix();

    QFile file(filePath);

    bool success = persistence::overwrite(file, static_cast<std::function<bool(QTextStream&)>>([&](QTextStream& stream)
    {
        if (fileType == "md") {
            MarkdownParser(m_document).write(stream);
        } else if (fileType.contains("htm")) {
            file.write(m_document->toHtml().toUtf8());
        } else {
            file.write(m_document->toPlainText().toUtf8());
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

void FormattableTextArea::copy()
{
    if (m_textCursor.hasSelection()) {
        QClipboard* clipboard = QGuiApplication::clipboard();
        QMimeData* mimeData = new QMimeData();
        mimeData->setHtml(m_textCursor.selection().toHtml("utf-8"));
        clipboard->setMimeData(mimeData);
    }
}

void FormattableTextArea::paste()
{
    const QMimeData* mimeData = QGuiApplication::clipboard()->mimeData();

    if (mimeData->hasHtml()) {
        // TODO: Remove font information from HTML before pasting
        m_textCursor.insertHtml(mimeData->html());
    } else if (mimeData->hasText()) {
        m_textCursor.insertText(mimeData->text());
    }

    update();
    emit caretPositionChanged();

    this->updateWordCount();
}

void FormattableTextArea::undo()
{
    m_document->undo(&m_textCursor);
    update();
    emit textChanged();
    emit caretPositionChanged();

    this->updateWordCount();
}

void FormattableTextArea::redo()
{
    m_document->redo(&m_textCursor);
    update();
    emit textChanged();
    emit caretPositionChanged();

    this->updateWordCount();
}

void FormattableTextArea::moveCursor(QTextCursor::MoveOperation op, QTextCursor::MoveMode mode, int by)
{
    m_textCursor.movePosition(op, mode, by);
    update();
    emit caretPositionChanged();
}

void FormattableTextArea::mergeFormat(const QTextCharFormat &format)
{
    m_textCursor.mergeCharFormat(format);
}
