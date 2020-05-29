#include "symbols.h"

bool symbols::isWordSeparator(const QChar& character)
{
    return !character.isLetterOrNumber() && !word_separators_multiple.contains(character);
}

QString symbols::sanitize(const QString& string)
{
    QString newString;

    foreach (const QChar& character, string) {
        if (character.isPrint() || character.isSpace()) {
            newString += character;
        }
    }

    return newString;
}
