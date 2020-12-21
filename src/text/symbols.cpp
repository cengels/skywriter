#include "symbols.h"

bool symbols::isWordSeparator(const QChar& character)
{
    return !character.isLetterOrNumber() && !word_separators_multiple.contains(character);
}

bool symbols::isNewLine(const QChar& character) {
    return character == symbols::newline
        || character == symbols::carriage_return
        || character == symbols::paragraph_separator
        || character == QChar::SpecialCharacter::LineFeed
        || character == QChar::SpecialCharacter::LineSeparator;
}

QString symbols::sanitize(const QString& string)
{
    QString newString;

    for (const QChar& character : string) {
        if (character.isPrint() || character.isSpace()) {
            newString += character;
        }
    }

    return newString;
}

bool symbols::containsWordSeparator(const QString& string)
{
    for (const QChar& character : string) {
        if (isWordSeparator(character)) {
            return true;
        }
    }

    return false;
}

bool symbols::isApostrophe(const QChar& character)
{
    return character == simple_apostrophe || character == curly_apostrophe;
}
