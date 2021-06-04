#include <QTextBlock>
#include <QDebug>
#include <QTextDocumentFragment>
#include <QException>

#include "MarkdownParser.h"
#include "symbols.h"
#include "format.h"

namespace {
    QString escape(const QString& string)
    {
        QString newString;

        for (const QChar& character : string) {
            if (character == symbols::italic_mark
             || character == symbols::italic_mark_alt
             || character == symbols::strikethrough_mark
             || character == symbols::escape_mark)
            {
                newString += symbols::escape_mark;
            }

            newString += character;
        }

        return newString;
    }

    void writeBlankLine(QTextStream& stream)
    {
        stream << "<br/>" << symbols::newline << symbols::newline;
    }

    void writeFragment(QTextStream& stream, const QTextFragment fragment, QStack<QString>& marks, bool firstFragment)
    {
        const QTextCharFormat& format = fragment.charFormat();

        bool bold = format.fontWeight() > QFont::Bold;
        bool italic = format.fontItalic();
        bool strikethrough = format.fontStrikeOut();
        bool done = false;

        while (!done && !marks.isEmpty()) {
            const QString& top = marks.top();

            // Close all finished marks in the right order.

            if ((top == symbols::bold_mark && !bold)
             || (top == symbols::italic_mark && !italic)
             || (top == symbols::strikethrough_mark && !strikethrough)) {
                stream << marks.pop();
            } else {
                done = true;
            }
        }

        if (bold && !marks.contains(symbols::bold_mark)) {
            stream << symbols::bold_mark;
            marks.push(symbols::bold_mark);
        }

        if (italic && !marks.contains(symbols::italic_mark)) {
            stream << symbols::italic_mark;
            marks.push(symbols::italic_mark);
        }

        if (strikethrough && !marks.contains(symbols::strikethrough_mark)) {
            stream << symbols::strikethrough_mark;
            marks.push(symbols::strikethrough_mark);
        }

        if (firstFragment && fragment.text().startsWith('#')) {
            stream << '\\';
        }

        stream << escape(fragment.text());
    }

    void writeBlock(QTextStream& stream, const QTextBlock& block)
    {
        const QTextBlockFormat& format = block.blockFormat();

        if (block.blockFormat() == format::sceneBreakFormat) {
            stream << "---" << symbols::newline << symbols::newline;
            return;
        }

        if (block.text().isEmpty()) {
            writeBlankLine(stream);

            return;
        }

        if (format.headingLevel() > 0) {
            stream << QStringLiteral("#").repeated(format.headingLevel()) << " ";
        }

        QStack<QString> marks;

        bool firstFragment = true;
        for (QTextBlock::Iterator iterator = block.begin(); !iterator.atEnd(); iterator++) {
            writeFragment(stream, iterator.fragment(), marks, firstFragment);
            firstFragment = false;
        }

        while (!marks.isEmpty()) {
            stream << marks.pop();
        }

        stream << symbols::newline << symbols::newline;
    }
}

const QTextCharFormat MarkdownParser::CHAR_FORMAT_REGULAR = QTextCharFormat();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_EMPHASIS = []{
    QTextCharFormat format;
    format.setFontItalic(true);

    return format;
}();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_DOUBLE_EMPHASIS = []{
    QTextCharFormat format;
    format.setFontWeight(QFont::Weight::Bold);

    return format;
}();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_STRIKETHROUGH = []{
    QTextCharFormat format;
    format.setFontStrikeOut(true);

    return format;
}();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_EMPHASIS_INV = []{
    QTextCharFormat format;
    format.setFontItalic(false);

    return format;
}();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_DOUBLE_EMPHASIS_INV = []{
    QTextCharFormat format;
    format.setFontWeight(QFont::Weight::Normal);

    return format;
}();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_STRIKETHROUGH_INV = []{
    QTextCharFormat format;
    format.setFontStrikeOut(false);

    return format;
}();

MarkdownParser::MarkdownParser(QTextDocument* document) :
    m_document(document),
    m_textCursor(new QTextCursor(document)),
    m_formatStack(),
    m_flags(None)
{
    m_parse_info = {
        0, // abi_version
        unsigned(MD_FLAG_NOHTML | MD_FLAG_STRIKETHROUGH),
        &MarkdownParser::onEnterBlock,
        &MarkdownParser::onLeaveBlock,
        &MarkdownParser::onEnterSpan,
        &MarkdownParser::onLeaveSpan,
        &MarkdownParser::onText,
        nullptr,
        nullptr
    };
}

MarkdownParser::MarkdownParser(QTextCursor* cursor) :
    m_document(cursor->document()),
    m_textCursor(cursor),
    m_formatStack(),
    m_flags(None)
{
    m_parse_info = {
        0, // abi_version
        unsigned(MD_FLAG_NOHTML | MD_FLAG_STRIKETHROUGH),
        &MarkdownParser::onEnterBlock,
        &MarkdownParser::onLeaveBlock,
        &MarkdownParser::onEnterSpan,
        &MarkdownParser::onLeaveSpan,
        &MarkdownParser::onText,
        nullptr,
        nullptr
    };
}

void MarkdownParser::parse(const QString& string)
{
    m_document->setUndoRedoEnabled(false);
    m_document->clear();
    QByteArray byteArray = string.toUtf8();

    m_textCursor->beginEditBlock();
    md_parse(byteArray.constData(), MD_SIZE(byteArray.size()), &m_parse_info, this);
    m_textCursor->endEditBlock();
    m_document->setUndoRedoEnabled(true);
}

QString MarkdownParser::stringify() const
{
    QString string;
    QTextStream stream(&string);

    write(stream);

    return string;
}

void MarkdownParser::write(QTextStream& stream) const
{
    QTextBlock block = m_document->begin();

    while (block.isValid()) {
        writeBlock(stream, block);
        block = block.next();
    }
}

int MarkdownParser::onEnterBlock(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    MarkdownParser *parser = static_cast<MarkdownParser *>(userdata);
    return parser->onEnterBlock(type, detail);
}

int MarkdownParser::onLeaveBlock(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    MarkdownParser *parser = static_cast<MarkdownParser *>(userdata);
    return parser->onLeaveBlock(type, detail);
}

int MarkdownParser::onEnterSpan(MD_SPANTYPE type, void* detail, void* userdata)
{
    MarkdownParser *parser = static_cast<MarkdownParser *>(userdata);
    return parser->onEnterSpan(type, detail);
}

int MarkdownParser::onLeaveSpan(MD_SPANTYPE type, void* detail, void* userdata)
{
    MarkdownParser *parser = static_cast<MarkdownParser *>(userdata);
    return parser->onLeaveSpan(type, detail);
}

int MarkdownParser::onText(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
{
    MarkdownParser *parser = static_cast<MarkdownParser *>(userdata);
    return parser->onText(type, text, size);
}

int MarkdownParser::onEnterBlock(MD_BLOCKTYPE type, void* detail)
{
    if (type == MD_BLOCK_DOC) {
        m_flags |= ParserFlags::NoNewBlockNeeded;
        return 0;
    }

    if (type == MD_BLOCK_HR) {
        m_textCursor->insertBlock(format::sceneBreakFormat);
        return 0;
    }

    if (type == MD_BLOCK_H)
    {
        const auto* headingDetail = static_cast<MD_BLOCK_H_DETAIL*>(detail);
        const HeadingFormat& headingFormat = ThemeManager::instance()->activeTheme()->headingFormat(headingDetail->level);

        if (m_flags & ParserFlags::NoNewBlockNeeded) {
            m_flags &= ~ParserFlags::NoNewBlockNeeded;
            m_textCursor->setBlockFormat(headingFormat.blockFormat());
        } else {
            m_textCursor->insertBlock(headingFormat.blockFormat());
        }

        return 0;
    }

    if (m_flags & ParserFlags::NoNewBlockNeeded) {
        m_flags &= ~ParserFlags::NoNewBlockNeeded;
        m_textCursor->setBlockFormat(ThemeManager::instance()->activeTheme()->blockFormat());
    } else {
        m_textCursor->insertBlock(ThemeManager::instance()->activeTheme()->blockFormat());
    }

    return 0;
}

int MarkdownParser::onLeaveBlock(MD_BLOCKTYPE type, void* detail)
{
    Q_UNUSED(type);
    Q_UNUSED(detail);

    return 0;
}

int MarkdownParser::onEnterSpan(MD_SPANTYPE type, void* detail)
{
    Q_UNUSED(detail);

    toggleFormat(getCharFormat(type));

    return 0;
}

int MarkdownParser::onLeaveSpan(MD_SPANTYPE type, void* detail)
{
    Q_UNUSED(detail);

    // We can't just revert to regular format here.
    // Must take into account possible nested spans.

    switch (type) {
        case MD_SPAN_EM:
        case MD_SPAN_STRONG:
        case MD_SPAN_DEL:
            toggleFormat(getCharFormat(type));
            break;
        default: break;
    }

    return 0;
}

int MarkdownParser::onText(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size)
{
    QString string;
    switch (type) {
        case MD_TEXT_NULLCHAR:
            string = QString(QChar(0xFFFD)); // CommonMark-required replacement for null
            break;
        case MD_TEXT_BR:
            string = QString(symbols::newline);
            break;
        case MD_TEXT_SOFTBR:
            string = QString(symbols::space);
            break;
        default:
            string = QString::fromUtf8(text, int(size));
    }

    if (!string.isEmpty()) {
        if (string != "<br />" && string != "<br/>") {
            // If the line contains a <br/> tag or a variation thereof,
            // do nothing (i.e. leave the line empty).

            m_textCursor->insertText(string);
        }
    }

    return 0;
}

void MarkdownParser::toggleFormat(const QTextCharFormat& format)
{
    const QTextCharFormat& invertedFormat = inverted(format);

    if (m_formatStack.contains(invertedFormat)) {
        // Nested format of the same type or just regular ending.
        // Remove it from the stack and return to regular format.
        m_formatStack.removeOne(invertedFormat);
        m_textCursor->mergeCharFormat(invertedFormat);
    } else {
        m_formatStack.push(inverted(format));
        m_textCursor->mergeCharFormat(format);
    }
}

const QTextCharFormat& MarkdownParser::getCharFormat(MD_SPANTYPE type)
{
    switch (type) {
        case MD_SPAN_EM:
            return CHAR_FORMAT_EMPHASIS;
            break;
        case MD_SPAN_STRONG:
            return CHAR_FORMAT_DOUBLE_EMPHASIS;
            break;
        case MD_SPAN_DEL:
            return CHAR_FORMAT_STRIKETHROUGH;
            break;
        default: break;
    }
}

const QTextCharFormat& MarkdownParser::inverted(const QTextCharFormat& format)
{
    Q_ASSERT(format != CHAR_FORMAT_EMPHASIS_INV
          && format != CHAR_FORMAT_DOUBLE_EMPHASIS_INV
          && format != CHAR_FORMAT_STRIKETHROUGH_INV);

    if (format == CHAR_FORMAT_EMPHASIS) return CHAR_FORMAT_EMPHASIS_INV;
    if (format == CHAR_FORMAT_DOUBLE_EMPHASIS) return CHAR_FORMAT_DOUBLE_EMPHASIS_INV;
    if (format == CHAR_FORMAT_STRIKETHROUGH) return CHAR_FORMAT_STRIKETHROUGH_INV;

    throw QException();
}
