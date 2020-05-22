#include <set>
#include <QStringList>
#include <QString>
#include <QDebug>

#include "symbols.h"
#include "TextIterator.h"

namespace {
    bool isWordSeparator(const QString::const_iterator charIterator) {
        return std::any_of(symbols::word_separators.cbegin(), symbols::word_separators.cend(), [&charIterator](const QChar& separator) {
            return (*charIterator) == separator;
        }) || std::any_of(symbols::word_separators_multiple.cbegin(), symbols::word_separators_multiple.cend(), [&charIterator](const QChar& separator) {
            QString::const_iterator iteratorCopy(charIterator);
            // if current char is separator
            // and either next or previous char is separator
            return (*charIterator) == separator
                   && ((*(--iteratorCopy)) == separator
                   || (*(++(++iteratorCopy))) == separator);
        });
    }

    const QString nextWord(QString::const_iterator& charIterator, const QString::const_iterator& end)
    {
        QString word;

        if (charIterator == end) {
            return QString();
        }

        bool isSeparator = false;

        // Iterate until it finds a word separator, as long as the current word
        // is not empty (which means that the iteration began with a word
        // separator). Abort if the end of the string is found.

        while ((!((isSeparator = isWordSeparator(charIterator))) || word.isEmpty()) && charIterator != end) {
            QChar character = *charIterator;
            // The isLetter() check makes sure to exclude non-letter characters
            // from the output. That way free-standing symbols that are not
            // declared word separators don't get falsely interpreted as a word.
            if (!isSeparator && charIterator->isLetterOrNumber()) {
                word.append(character);
            }
            charIterator++;
        }

        return word;
    }
}

TextIterator::TextIterator(const QString& text, const IterationType iterationType) :
    m_text(text),
    m_iterationType(iterationType),
    m_charIterator(text.constBegin()),
    m_charIteratorEnd(text.constEnd()),
    m_current()
{
    switch (m_iterationType) {
        case TextIterator::IterationType::ByCharacter:
            m_current = QString(m_charIterator);
            break;
        case TextIterator::IterationType::ByWord:
            m_current = nextWord(m_charIterator, m_charIteratorEnd);
            break;
        default:
            Q_ASSERT(false); // not implemented
    }
}

TextIterator::TextIterator(const TextIterator& textIterator) :
    m_text(textIterator.m_text),
    m_iterationType(textIterator.m_iterationType),
    m_charIterator(textIterator.m_charIterator),
    m_charIteratorEnd(textIterator.m_charIteratorEnd),
    m_current(textIterator.m_current) { }

TextIterator& TextIterator::operator++()
{
    switch (m_iterationType) {
        case TextIterator::IterationType::ByCharacter:
            if (!atEnd()) {
                m_charIterator++;
                m_current = QString(m_charIterator);
            }
            break;
        case TextIterator::IterationType::ByWord:
            if (!atEnd()) {
                m_current = nextWord(m_charIterator, m_charIteratorEnd);
            }
            break;
        default:
            Q_ASSERT(false); // not implemented
    }

    return *this;
}

TextIterator TextIterator::operator++(int)
{
    TextIterator returnValue = *this;
    this->operator++();
    return returnValue;
}

TextIterator& TextIterator::operator+=(int by)
{
    for (int i = 0; i < by && !atEnd(); i++) {
        this->operator++();
    }

    return *this;
}

bool TextIterator::operator==(const TextIterator& other) const
{
    return this->m_iterationType == other.m_iterationType
            && this->m_charIterator == other.m_charIterator;
}

bool TextIterator::operator!=(const TextIterator& other) const
{
    return !(this->operator==(other));
}

const QString TextIterator::current() const
{
    return this->m_current;
}

bool TextIterator::atEnd() const {
    return m_current == '\0' || m_current.isNull();
}
