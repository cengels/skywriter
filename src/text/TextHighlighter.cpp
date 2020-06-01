#include <QSyntaxHighlighter>
#include <QDebug>

#include "TextHighlighter.h"
#include "symbols.h"
#include "../theming/ThemeManager.h"
#include "../colors.h"

namespace {
    QMetaObject::Connection connection;
}

TextHighlighter::TextHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent),
    m_refreshing(true)
{
    connection = connect(ThemeManager::instance()->activeTheme(), &Theme::fontColorChanged, this, &TextHighlighter::refresh);
    connect(ThemeManager::instance(), &ThemeManager::activeThemeChanged, this, [&]() {
        disconnect(connection);
        connection = connect(ThemeManager::instance()->activeTheme(), &Theme::fontColorChanged, this, &TextHighlighter::rehighlight);

        this->refresh();
    });
}

void TextHighlighter::highlightBlock(const QString& text)
{
    highlightHeadings();
    highlightComments(text);
}

void TextHighlighter::setCurrentBlockStateFlag(TextHighlighter::BlockState state)
{
    setCurrentBlockState(currentBlockState() | state);
}

bool TextHighlighter::checkPreviousBlockStateFlag(TextHighlighter::BlockState state) const
{
    if (previousBlockState() == -1) {
        return false;
    }

    return previousBlockState() & state;
}

void TextHighlighter::highlightComments(const QString& text)
{
    setCurrentBlockState(BlockState::None);

    int startIndex = checkPreviousBlockStateFlag(BlockState::EndsWithUnclosedComment)
                     ? 0
                     : text.indexOf(symbols::opening_comment);

    const QColor& fontColor = ThemeManager::instance()->activeTheme()->fontColor();
    const QColor commentColor = colors::decreaseEmphasis(fontColor);

    while (startIndex >= 0) {
        int endIndex = text.indexOf(symbols::closing_comment, startIndex);
        int commentLength = 0;

        if (endIndex == -1) {
            setCurrentBlockStateFlag(BlockState::EndsWithUnclosedComment);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + 1;
        }

        setFormat(startIndex, commentLength, commentColor);
        startIndex = text.indexOf(symbols::opening_comment, startIndex + commentLength);
    }
}

void TextHighlighter::highlightHeadings()
{
    int headingLevel = currentBlock().blockFormat().headingLevel();
    if (headingLevel > 0) {
        setFormat(0,
                  currentBlock().length() - 1,
                  ThemeManager::instance()->activeTheme()->headingFormat(headingLevel).charFormat());
    }
}

void TextHighlighter::refresh()
{
    this->m_refreshing = true;
    this->rehighlight();
    this->m_refreshing = false;
}

bool TextHighlighter::refreshing() const {
    return this->m_refreshing;
}
