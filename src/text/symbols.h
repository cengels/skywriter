#include <QChar>
#include <set>

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
    //! Identifies all possible tokens that force the parser to consider
    //! the next token part of a new word.
    //!
    //! Note that these are not the symbols that are ignored during word
    //! counting! The parser must iterate through all separators on a per-
    //! character-basis. For that reason, it is best to keep this set as
    //! concise and small as possible.
    const std::set<QChar> word_separators { ' ', newline, em_dash, en_dash };
    //! Identifies tokens that force the parser to consider the next token part
    //! of a new word only if this token appears multiple times in a row.
    //! Single occurrences are ignored.
    const std::set<QChar> word_separators_multiple { '-' };
}

#endif // SYMBOLS_H
