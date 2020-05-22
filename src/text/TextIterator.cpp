#include <set>
#include <QStringList>
#include <QString>
#include <QDebug>

#include "symbols.h"
#include "TextIterator.h"

namespace {
    bool containsFirst(const QSet<const QPair<const QChar, const QChar>> set, const QChar& character) {
        return std::any_of(set.cbegin(), set.cend(), [&character](const QPair<const QChar, const QChar>& pair) {
            return pair.first == character;
        });
    }
    bool containsSecond(const QSet<const QPair<const QChar, const QChar>> set, const QChar& character) {
        return std::any_of(set.cbegin(), set.cend(), [&character](const QPair<const QChar, const QChar>& pair) {
            return pair.second == character;
        });
    }

    bool isWordSeparator(const QString::const_iterator charIterator) {
        return symbols::word_separators.contains(*charIterator)
        || std::any_of(symbols::word_separators_multiple.cbegin(), symbols::word_separators_multiple.cend(), [&charIterator](const QChar& separator) {
            QString::const_iterator iteratorCopy(charIterator);
            // if current char is separator
            // and either next or previous char is separator
            return (*charIterator) == separator
                   && ((*(--iteratorCopy)) == separator
                   || (*(++(++iteratorCopy))) == separator);
        });
    }
}

TextIterator::TextIterator(const QString& text, const IterationType iterationType) :
    m_text(text),
    m_iterationType(iterationType),
    m_charIterator(text.constBegin()),
    m_charIteratorEnd(text.constEnd()),
    m_current(),
    m_ignorePairs(),
    m_state(TextIteratorState::None)
{
    switch (m_iterationType) {
        case TextIterator::IterationType::ByCharacter:
            m_current = this->nextChar();
            break;
        case TextIterator::IterationType::ByWord:
            m_current = this->nextWord();
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
    m_current(textIterator.m_current),
    m_ignorePairs(textIterator.m_ignorePairs),
    m_state(textIterator.m_state) { }

TextIterator& TextIterator::operator++()
{
    switch (m_iterationType) {
        case TextIterator::IterationType::ByCharacter:
            if (!atEnd()) {
                m_current = this->nextChar();
            }
            break;
        case TextIterator::IterationType::ByWord:
            if (!atEnd()) {
                m_current = this->nextWord();
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

void TextIterator::ignoreEnclosedBy(const QChar openingToken, const QChar closingToken) {
    this->m_ignorePairs.insert(QPair<const QChar, const QChar>(openingToken, closingToken));
}

const QString TextIterator::nextChar()
{
    do {
        if (m_charIterator == m_charIteratorEnd) {
            return QString();
        }

        m_charIterator++;

        this->updateState();
    } while (m_state & TextIteratorState::IgnoreBlock);

    return QString(*m_charIterator);
}

const QString TextIterator::nextWord()
{
    QString word;

    if (m_charIterator == m_charIteratorEnd) {
        return QString();
    }

    // Iterate until it finds a word separator, as long as the current word
    // is not empty (which means that the iteration began with a word
    // separator). Abort if the end of the string is found.

    while (m_charIterator != m_charIteratorEnd) {
        this->updateState();

        if (m_state & TextIteratorState::IgnoreBlock) {
            m_charIterator++;
            continue;
        }

        bool isSeparator = isWordSeparator(m_charIterator);

        if (isSeparator && !word.isEmpty()) {
            break;
        }

        QChar character = *m_charIterator;

        // The isLetter() check makes sure to exclude non-letter characters
        // from the output. That way free-standing symbols that are not
        // declared word separators don't get falsely interpreted as a word.

        if (!isSeparator && m_charIterator->isLetterOrNumber()) {
            word.append(character);
        }
        m_charIterator++;
    }

    return word;
}

void TextIterator::updateState()
{
    QChar character = *m_charIterator;

    if (m_state & TextIteratorState::IgnoreBlock && containsSecond(this->m_ignorePairs, character)) {
        m_state = TextIteratorState(m_state & ~TextIteratorState::IgnoreBlock);
    } else if (!(m_state & TextIteratorState::IgnoreBlock) && containsFirst(this->m_ignorePairs, character)) {
        m_state = TextIteratorState(m_state | TextIteratorState::IgnoreBlock);
    }
}
