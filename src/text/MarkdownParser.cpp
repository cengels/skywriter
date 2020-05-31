#include <QTextBlock>
#include <QDebug>
#include <QTextDocumentFragment>

#include "MarkdownParser.h"
#include "symbols.h"

const QTextCharFormat MarkdownParser::CHAR_FORMAT_REGULAR = QTextCharFormat();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_EMPHASIS = []{
    QTextCharFormat format;
    format.setFontItalic(true);

    return format;
}();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_DOUBLE_EMPHASIS = []{
    QTextCharFormat format;
    format.setFontWeight(700);

    return format;
}();

const QTextCharFormat MarkdownParser::CHAR_FORMAT_STRIKETHROUGH = []{
    QTextCharFormat format;
    format.setFontStrikeOut(true);

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
    m_document->clear();
    QByteArray byteArray = string.toUtf8();

    m_textCursor->beginEditBlock();
    md_parse(byteArray.constData(), MD_SIZE(byteArray.size()), &m_parse_info, this);
    m_textCursor->endEditBlock();
}

QString MarkdownParser::stringify() const
{
    return "";
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

    if (type == MD_BLOCK_H)
    {
        const auto* headingDetail = static_cast<MD_BLOCK_H_DETAIL*>(detail);
        const HeadingFormat& headingFormat = ThemeManager::instance()->activeTheme()->headingFormat(headingDetail->level);

        if (m_flags & ParserFlags::NoNewBlockNeeded) {
            m_flags &= ~ParserFlags::NoNewBlockNeeded;
            m_textCursor->setBlockFormat(headingFormat.blockFormat());
            m_textCursor->setBlockCharFormat(headingFormat.charFormat());
        } else {
            m_textCursor->insertBlock(headingFormat.blockFormat(), headingFormat.charFormat());
        }

        return 0;
    }

    // TODO: Assemble format from activeTheme
    if (m_flags & ParserFlags::NoNewBlockNeeded) {
        m_flags &= ~ParserFlags::NoNewBlockNeeded;
        m_textCursor->setBlockFormat(QTextBlockFormat());
        m_textCursor->setBlockCharFormat(QTextCharFormat());
    } else {
        m_textCursor->insertBlock(QTextBlockFormat(), QTextCharFormat());
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

    switch (type) {
        case MD_SPAN_EM:
            startFormat(CHAR_FORMAT_EMPHASIS);
            break;
        case MD_SPAN_STRONG:
            startFormat(CHAR_FORMAT_DOUBLE_EMPHASIS);
            break;
        case MD_SPAN_DEL:
            startFormat(CHAR_FORMAT_STRIKETHROUGH);
            break;
        default: break;
    }

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
            if (m_formatStack.isEmpty()) {
                // This is not allowed to happen. The stack must at least
                // contain the current format.
                return -1;
            }

            endFormat();
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
        if (!string.startsWith("<br") || !string.endsWith(">")) {
            // If the line contains a <br/> tag or a variation thereof,
            // do nothing (i.e. leave the line empty).

            m_textCursor->insertText(string);
        }
    }

    return 0;
}

void MarkdownParser::startFormat(const QTextCharFormat& format)
{
    m_formatStack.push(m_textCursor->charFormat());
    m_textCursor->mergeCharFormat(format);
}

void MarkdownParser::endFormat()
{
    Q_ASSERT(!m_formatStack.isEmpty());

    m_textCursor->setCharFormat(m_formatStack.pop());
}