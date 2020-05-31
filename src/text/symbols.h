#include <QChar>
#include <QSet>

#ifndef SYMBOLS_H
#define SYMBOLS_H

namespace symbols {
    //! Identifies the token before a comment begins.
    constexpr QChar opening_comment = '[';
    //! Identifies the token after a comment ends.
    constexpr QChar closing_comment = ']';
    constexpr QChar en_dash = 0x2013;
    constexpr QChar em_dash = 0x2014;
    constexpr QChar newline = '\n';
    constexpr QChar space = ' ';
    const QString bold_mark = "**";
    constexpr QChar italic_mark = '*';
    constexpr QChar strikethrough_mark = '~';
    //! Identifies all possible tokens that force the parser to consider
    //! the next token part of a new word.
    //!
    //! Note that these are not the symbols that are ignored during word
    //! counting! The parser must iterate through all separators on a per-
    //! character-basis. For that reason, it is best to keep this set as
    //! concise and small as possible.
    const QSet<QChar> word_separators { space, newline, em_dash, en_dash };
    //! Identifies tokens that force the parser to consider the next token part
    //! of a new word only if this token appears multiple times in a row.
    //! Single occurrences are ignored.
    const QSet<QChar> word_separators_multiple { '-' };
    //! Checks if the given character is a word separator.
    bool isWordSeparator(const QChar& character);
    //! Removes unprintable characters from the string.
    QString sanitize(const QString& string);
}

#endif // SYMBOLS_H
