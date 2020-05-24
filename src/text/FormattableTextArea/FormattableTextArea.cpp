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

#include "FormattableTextArea.h"
#include "../format.h"
#include "../symbols.h"
#include "../TextHighlighter.h"

namespace {
    constexpr QTextDocument::MarkdownFeatures MARKDOWN_FEATURES = QTextDocument::MarkdownNoHTML;
}

FormattableTextArea::FormattableTextArea(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_document(nullptr)
    , m_textLayout(new QTextLayout())
    , m_highlighter(nullptr)
    , m_cursorPosition(-1)
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_characterCount(0)
    , m_wordCount(0)
    , m_paragraphCount(0)
    , m_pageCount(0)
    , m_firstLineIndent(0.0)
{
    setFiltersChildMouseEvents(true);
    setAcceptedMouseButtons(Qt::MouseButton::AllButtons);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    QCursor cursor;
    cursor.setShape(Qt::CursorShape::IBeamCursor);
    setCursor(cursor);

    newDocument();

    connect(this, &FormattableTextArea::widthChanged, this, [this]() {
        this->m_document->setTextWidth(this->width());
        update();
    });

    connect(this, &FormattableTextArea::heightChanged, this, [this]() {
        update();
    });
}

void FormattableTextArea::paint(QPainter *painter)
{
    if (m_document) {
//        m_textLayout->setText(this.te)
//        qreal margin = 10;
//        qreal radius = qMin(width()/2.0, height()/2.0) - margin;
//        QFontMetrics fm(font);

//        qreal lineHeight = fm.height();
//        qreal y = 0;

//        textLayout.beginLayout();

//        while (1) {
//            // create a new line
//            QTextLine line = textLayout.createLine();
//            if (!line.isValid())
//                break;

//            qreal x1 = qMax(0.0, pow(pow(radius,2)-pow(radius-y,2), 0.5));
//            qreal x2 = qMax(0.0, pow(pow(radius,2)-pow(radius-(y+lineHeight),2), 0.5));
//            qreal x = qMax(x1, x2) + margin;
//            qreal lineWidth = (width() - margin) - x;

//            line.setLineWidth(lineWidth);
//            line.setPosition(QPointF(x, margin+y));
//            y += line.height();
//        }

//        textLayout.endLayout();

//        QPainter painter;
//        painter.begin(this);
//        painter.setRenderHint(QPainter::Antialiasing);
//        painter.fillRect(rect(), Qt::white);
//        painter.setBrush(QBrush(Qt::black));
//        painter.setPen(QPen(Qt::black));
//        textLayout.draw(&painter, QPoint(0,0));

//        painter.setBrush(QBrush(QColor("#a6ce39")));
//        painter.setPen(QPen(Qt::black));
//        painter.drawEllipse(QRectF(-radius, margin, 2*radius, 2*radius));
//        painter.end();
        m_document->drawContents(painter);
    }
}

bool FormattableTextArea::event(QEvent* event)
{
    qDebug() << event->type();

    if (event->type() == QEvent::MouseButtonPress) {
        forceActiveFocus();
    }

    event->ignore();

    return true;
}

bool FormattableTextArea::childMouseEventFilter(QQuickItem *item, QEvent *event)
{
    return this->event(event);
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

        auto textOption = m_document->defaultTextOption();
        textOption.setWrapMode(QTextOption::WordWrap);
        m_document->setDefaultTextOption(textOption);
        this->m_document->setTextWidth(this->width());
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
    // TODO: Speed this up ... a lot
    // Suggestion: Create a QTextDocument first and then use QTextEdit::setDocument() to make QTextEdit display it. It should operate much faster on the text then.
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
        if (QTextDocument *doc = new QTextDocument(this)) {
            const auto text = codec->toUnicode(data);
            const QString fileType = QFileInfo(file).suffix();

            if (fileType == "md") {
                qDebug() << "foo";
                doc->setMarkdown(text, MARKDOWN_FEATURES);
            } else if (fileType.contains("htm")) {
                doc->setHtml(text);
            } else {
                doc->setPlainText(text);
            }

            doc->setModified(false);
            newDocument(doc);
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
    if (!m_document)
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
        file.write(m_document->toMarkdown(MARKDOWN_FEATURES).toUtf8());
    } else if (fileType.contains("htm")) {
        file.write(m_document->toHtml().toUtf8());
    } else {
        file.write(m_document->toPlainText().toUtf8());
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
    if (!m_document)
        return QTextCursor();

    QTextCursor cursor = QTextCursor(m_document);
    if (m_selectionStart != m_selectionEnd) {
        cursor.setPosition(m_selectionStart);
        cursor.setPosition(m_selectionEnd, QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(m_cursorPosition);
    }
    return cursor;
}

void FormattableTextArea::mergeFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = textCursor();
    cursor.mergeCharFormat(format);
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

TextIterator FormattableTextArea::wordIterator() const
{
    TextIterator iterator = TextIterator(m_document->toPlainText(), TextIterator::IterationType::ByWord);
    iterator.ignoreEnclosedBy(symbols::opening_comment, symbols::closing_comment);

    return iterator;
}

double FormattableTextArea::contentWidth() const
{
    return m_document ? m_document->size().width() : -1;
}

double FormattableTextArea::contentHeight() const
{
    return m_document ? m_document->size().height() : -1;
}
