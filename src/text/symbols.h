#include <QChar>
#include <QSet>

#ifndef SYMBOLS_H
#define SYMBOLS_H

namespace symbols {
    //! Identifies the token before a comment begins.
    constexpr QChar opening_comment = '[';
    //! Identifies the token after a comment ends.
    constexpr QChar closing_comment = ']';
    constexpr QChar hyphen = '-';
    constexpr QChar en_dash = 0x2013;
    constexpr QChar em_dash = 0x2014;
    constexpr QChar newline = '\n';
    constexpr QChar carriage_return = '\r';
    constexpr QChar paragraph_separator = QChar(QChar::SpecialCharacter::ParagraphSeparator);
    constexpr QChar space = ' ';
    const QString bold_mark = "**";
    const QString bold_mark_alt = "__";
    constexpr QChar italic_mark = '*';
    constexpr QChar italic_mark_alt = '_';
    constexpr QChar strikethrough_mark = '~';
    constexpr QChar escape_mark = '\\';
    constexpr QChar simple_apostrophe = '\'';
    constexpr QChar curly_apostrophe = 0x2019;
    //! Identifies all possible tokens that force the parser to consider
    //! the next token part of a new word.
    //!
    //! Note that these are not the symbols that are ignored during word
    //! counting! The parser must iterate through all separators on a per-
    //! character-basis. For that reason, it is best to keep this set as
    //! concise and small as possible.
    const QSet<QChar> word_separators { space, newline, paragraph_separator, carriage_return, em_dash, en_dash };
    //! Identifies tokens that force the parser to consider the next token part
    //! of a new word only if this token appears multiple times in a row.
    //! Single occurrences are ignored.
    const QSet<QChar> word_separators_multiple { hyphen, simple_apostrophe, curly_apostrophe };
    //! Identifies tokens as a subset of word_separators_multiple that should *not*
    //! be ignored during word-by-word selections, i.e. the word selection will stop
    //! if it encounters a token in this set.
    const QSet<QChar> stop_on_selection { hyphen };
    //! Identifies all tokens after which the StringReplacer should use the
    //! opening replacement of a smart replacement rather than the closing one.
    const QSet<QChar> smart_replacement_prefixes { space, newline, paragraph_separator, carriage_return };
    //! Checks if the given character is a word separator.
    bool isWordSeparator(const QChar& character);
    //! Checks if a given character is an apostrophe.
    bool isApostrophe(const QChar& character);
    //! Checks if the string contains a word separator.
    bool containsWordSeparator(const QString& string);
    //! Removes unprintable characters from the string.
    QString sanitize(const QString& string);
}

#endif // SYMBOLS_H
