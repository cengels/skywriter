#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <QDebug>

#include "TextFormatter.h"
#include "symbols.h"
#include "../theming/ThemeManager.h"
#include "../colors.h"
#include "../Range.h"
#include "format.h"
#include "UserData.h"

namespace {
    QMetaObject::Connection connection;

    QTextLayout::FormatRange formatRange(const int start, const int length, const QTextCharFormat& format) {
        QTextLayout::FormatRange formatRange;
        formatRange.start = start;
        formatRange.length = length;
        formatRange.format = format;

        return formatRange;
    }
}

TextFormatter::TextFormatter(QTextDocument* parent) : QSyntaxHighlighter(parent),
    m_refreshing(true),
    m_findRanges(nullptr)
{
    connection = connect(ThemeManager::instance()->activeTheme(), &Theme::fontColorChanged, this, &TextFormatter::refresh);
    connect(ThemeManager::instance(), &ThemeManager::activeThemeChanged, this, [&]() {
        disconnect(connection);
        connection = connect(ThemeManager::instance()->activeTheme(), &Theme::fontColorChanged, this, &TextFormatter::refresh);

        this->refresh();
    });
}

void TextFormatter::highlightBlock(const QString& text)
{
    formatHeadings();
    formatComments(text);

    mergeFormats();
}

void TextFormatter::setCurrentBlockStateFlag(format::BlockState state)
{
    if (currentBlockState() == -1) {
        setCurrentBlockState(state);
    } else {
        setCurrentBlockState(currentBlockState() | state);
    }
}

void TextFormatter::unsetCurrentBlockStateFlag(format::BlockState state)
{
    if (currentBlockState() == -1) {
        setCurrentBlockState(format::BlockState::None);
    } else {
        setCurrentBlockState(currentBlockState() & ~state);
    }
}

bool TextFormatter::checkPreviousBlockStateFlag(format::BlockState state) const
{
    if (previousBlockState() == -1) {
        return false;
    }

    return previousBlockState() & state;
}

bool TextFormatter::checkCurrentBlockStateFlag(format::BlockState state) const
{
    if (currentBlockState() == -1) {
        return false;
    }

    return currentBlockState() & state;
}

void TextFormatter::formatHeadings()
{
    int headingLevel = currentBlock().blockFormat().headingLevel();
    if (headingLevel > 0) {
        setCurrentBlockStateFlag(format::Heading);
        setBlockFormat(ThemeManager::instance()->activeTheme()->headingFormat(headingLevel).charFormat());
    }
}

void TextFormatter::formatComments(const QString& text)
{
    int startIndex = checkPreviousBlockStateFlag(format::EndsWithUnclosedComment)
                     ? 0
                     : text.indexOf(symbols::opening_comment);

    const QColor& fontColor = ThemeManager::instance()->activeTheme()->fontColor();
    const QColor commentColor = colors::decreaseEmphasis(fontColor);

    if (startIndex == -1) {
        unsetCurrentBlockStateFlag(format::EndsWithUnclosedComment);
    }

    UserData* userData = dynamic_cast<UserData*>(this->currentBlock().userData());

    if (userData == nullptr) {
        userData = new UserData();
    }

    bool hadComments = !userData->comments().isEmpty();
    userData->clearCommentRanges();

    while (startIndex >= 0) {
        int endIndex = text.indexOf(symbols::closing_comment, startIndex);
        int commentLength = 0;

        if (endIndex == -1) {
            setCurrentBlockStateFlag(format::EndsWithUnclosedComment);
            commentLength = text.length() - startIndex;
        } else {
            unsetCurrentBlockStateFlag(format::EndsWithUnclosedComment);
            commentLength = endIndex - startIndex + 1;
        }

        setColor(startIndex, commentLength, commentColor);
        userData->addCommentRange(startIndex, startIndex + commentLength);
        startIndex = text.indexOf(symbols::opening_comment, startIndex + commentLength);
    }

    setCurrentBlockUserData(userData);

    if (userData->comments().length() > 0 || hadComments) {
        emit blockInvalidated(this->currentBlock());
    }
}

void TextFormatter::setCharFormat(int blockPosition, int length, const QTextCharFormat& format)
{
    QTextLayout::FormatRange range = formatRange(blockPosition, length, format);
    Range<int> mathRange(blockPosition, blockPosition + length);

    if (m_formats.isEmpty()) {
        m_formats.append(range);
    } else {
        for (int i = 0, size = m_formats.size();  i < size;  i++) {
            QTextLayout::FormatRange currentRange = m_formats.value(i);
            const Range<int> intersectRange = mathRange.intersect(currentRange.start, currentRange.start + currentRange.length);

            if (intersectRange.isValid()) {
                if (currentRange.start != blockPosition) {
                    // The two ranges do not have the same start.
                    QTextLayout::FormatRange startRange = currentRange.start < blockPosition ? currentRange : range;
                    startRange.length = intersectRange.from() - startRange.start;
                    m_formats.insert(i, startRange);
                    i++;
                }

                if (currentRange.start + currentRange.length != blockPosition + length) {
                    // The two ranges do not have the same end.
                    QTextLayout::FormatRange endRange = currentRange.start + currentRange.length > blockPosition + length ? currentRange : range;
                    const int end = endRange.start + endRange.length;
                    endRange.start = intersectRange.until();
                    endRange.length = end - endRange.start;
                    m_formats.insert(i + 1, endRange);
                }

                currentRange.start = intersectRange.from();
                currentRange.length = intersectRange.length();
                currentRange.format.merge(format);
                // We cannot return a reference from m_formats and then simply modify
                // it here instead of replacing it. If we try that, we'll get a
                // heap corruption. Why do we get a heap corruption, you ask?
                // I have no damn idea.
                m_formats.replace(i, currentRange);

                // We can return here because we know that, if a range intersects
                // with a range from the QVector, it cannot intersect with another
                // one since we hopefully merged them all properly.

                return;
            }
        }

        m_formats.append(range);
    }
}

void TextFormatter::setColor(int blockPosition, int length, const QColor& color)
{
    QTextCharFormat format;
    format.setForeground(color);
    setCharFormat(blockPosition, length, format);
}

void TextFormatter::setBackgroundColor(int blockPosition, int length, const QColor& color)
{
    QTextCharFormat format;
    format.setBackground(color);
    setCharFormat(blockPosition, length, format);
}

void TextFormatter::setBlockFormat(const QTextCharFormat& format)
{
    setCharFormat(0, currentBlock().length() - 1, format);
}

void TextFormatter::mergeFormats()
{
    for (const QTextLayout::FormatRange& formatRange : m_formats) {
        setFormat(formatRange.start, formatRange.length, formatRange.format);
    }

    m_formats.clear();
}

void TextFormatter::refresh()
{
    this->m_refreshing = true;
    this->rehighlight();
    this->m_refreshing = false;
}

bool TextFormatter::refreshing() const
{
    return this->m_refreshing;
}
