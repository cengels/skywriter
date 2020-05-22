#include <QSyntaxHighlighter>

#include "TextHighlighter.h"
#include "symbols.h"
#include "../theming/ThemeManager.h"

namespace {
    QMetaObject::Connection connection;
}

TextHighlighter::TextHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent),
    m_comments(0),
    m_refreshing(true)
{
    connection = connect(ThemeManager::instance()->activeTheme(), &Theme::fontColorChanged, this, &TextHighlighter::refresh);
    connect(ThemeManager::instance(), &ThemeManager::activeThemeChanged, this, [&]() {
        disconnect(connection);
        connection = connect(ThemeManager::instance()->activeTheme(), &Theme::fontColorChanged, this, &TextHighlighter::rehighlight);

        this->refresh();
    });
}

void TextHighlighter::highlightBlock(const QString& text) {
    // 0 = block does not end with an unclosed comment
    setCurrentBlockState(0);

    int startIndex = previousBlockState() == 1
                     ? 0
                     : text.indexOf(symbols::opening_comment);

    while (startIndex >= 0) {
        int endIndex = text.indexOf(symbols::closing_comment, startIndex);
        int commentLength = 0;

        if (endIndex == -1) {
            // 1 = block ends with unclosed comment
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + 1;
        }

        setFormat(startIndex, commentLength, ThemeManager::instance()->activeTheme()->fontColor().darker(150));
        startIndex = text.indexOf(symbols::opening_comment, startIndex + commentLength);
    }
}

void TextHighlighter::refresh() {
    this->m_refreshing = true;
    this->rehighlight();
    this->m_refreshing = false;
}

bool TextHighlighter::refreshing() const {
    return this->m_refreshing;
}

int TextHighlighter::comments() const {
    return this->m_comments;
}
