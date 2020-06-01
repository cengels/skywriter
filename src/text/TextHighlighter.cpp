#include <QSyntaxHighlighter>
#include <QDebug>

#include "TextHighlighter.h"
#include "symbols.h"
#include "../theming/ThemeManager.h"
#include "../colors.h"
#include "format.h"

namespace {
    QMetaObject::Connection connection;
}

TextHighlighter::TextHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent),
    m_refreshing(true),
    m_sceneBreakString()
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
    highlightSceneBreaks(text);
}

void TextHighlighter::setCurrentBlockStateFlag(format::BlockState state)
{
    if (currentBlockState() == -1) {
        setCurrentBlockState(state);
    } else {
        setCurrentBlockState(currentBlockState() | state);
    }
}

void TextHighlighter::unsetCurrentBlockStateFlag(format::BlockState state)
{
    if (currentBlockState() == -1) {
        setCurrentBlockState(format::BlockState::None);
    } else {
        setCurrentBlockState(currentBlockState() & ~state);
    }
}

bool TextHighlighter::checkPreviousBlockStateFlag(format::BlockState state) const
{
    if (previousBlockState() == -1) {
        return false;
    }

    return previousBlockState() & state;
}

bool TextHighlighter::checkCurrentBlockStateFlag(format::BlockState state) const
{
    if (currentBlockState() == -1) {
        return false;
    }

    return currentBlockState() & state;
}

void TextHighlighter::highlightComments(const QString& text)
{
    int startIndex = checkPreviousBlockStateFlag(format::EndsWithUnclosedComment)
                     ? 0
                     : text.indexOf(symbols::opening_comment);

    const QColor& fontColor = ThemeManager::instance()->activeTheme()->fontColor();
    const QColor commentColor = colors::decreaseEmphasis(fontColor);

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

        setFormat(startIndex, commentLength, commentColor);
        startIndex = text.indexOf(symbols::opening_comment, startIndex + commentLength);
    }
}

void TextHighlighter::highlightHeadings()
{
    int headingLevel = currentBlock().blockFormat().headingLevel();
    if (headingLevel > 0) {
        setCurrentBlockStateFlag(format::Heading);
        setBlockFormat(ThemeManager::instance()->activeTheme()->headingFormat(headingLevel).charFormat());
    }
}

void TextHighlighter::highlightSceneBreaks(const QString &text)
{
    if (text == m_sceneBreakString) {
        QTextCharFormat charFormat;
        charFormat.setFontPointSize(ThemeManager::instance()->activeTheme()->fontSize() * 1.25);
        setBlockFormat(charFormat);
    }
}

void TextHighlighter::setBlockFormat(const QTextCharFormat& format)
{
    setFormat(0, currentBlock().length() - 1, format);
}

void TextHighlighter::refresh()
{
    this->m_refreshing = true;
    this->rehighlight();
    this->m_refreshing = false;
}

const QString& TextHighlighter::sceneBreak() const
{
    return m_sceneBreakString;
}

void TextHighlighter::setSceneBreak(const QString sceneBreakString)
{
    m_sceneBreakString = sceneBreakString;
}

bool TextHighlighter::refreshing() const {
    return this->m_refreshing;
}
