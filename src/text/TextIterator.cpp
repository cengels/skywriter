#include <set>
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>

#include "symbols.h"
#include "../Range.h"
#include "../profiling.h"
#include "UserData.h"
#include "format.h"
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

// A previous approach considered using a copied QTextCursor instead of m_iterator here to iterate
// through the text. However, each call of QTextCursor::movePosition() or QTextCursor::setPosition()
// takes a significant period of time. A simple iterator is much faster.
// We also need to make sure that we use iterators to a copy of the selectedText, not to a reference
// otherwise everything breaks.
TextIterator::TextIterator(const QTextCursor& cursor, const IterationType iterationType) :
    m_originalCursor(cursor),
    m_text(cursor.selectedText()),
    m_iterator(m_text.constBegin()),
    m_iteratorEnd(m_text.constEnd()),
    m_position(cursor.selectionStart()),
    m_iterationType(iterationType),
    m_current(),
    m_commentsEnabled(false)
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
    m_originalCursor(textIterator.m_originalCursor),
    m_text(textIterator.m_text),
    m_iterator(textIterator.m_iterator),
    m_iteratorEnd(textIterator.m_iteratorEnd),
    m_position(textIterator.m_position),
    m_iterationType(textIterator.m_iterationType),
    m_current(textIterator.m_current),
    m_commentsEnabled(textIterator.m_commentsEnabled) { }

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
            && this->m_iterator == other.m_iterator;
}

bool TextIterator::operator!=(const TextIterator& other) const
{
    return !(this->operator==(other));
}

const QString TextIterator::current() const
{
    return this->m_current;
}

bool TextIterator::atEnd() const
{
    return m_current == '\0' || m_current.isNull();
}

const QString TextIterator::nextChar()
{
    do {
        if (m_iterator == m_iteratorEnd) {
            return QString();
        }

        m_iterator++;
        m_position++;
    } while (this->shouldIgnoreToken());

    return QString(*m_iterator);
}

const QString TextIterator::nextWord()
{
    QString word;

    if (m_iterator == m_iteratorEnd) {
        return QString();
    }

    // Iterate until it finds a word separator, as long as the current word
    // is not empty (which means that the iteration began with a word
    // separator). Abort if the end of the string is found.

    while (m_iterator != m_iteratorEnd) {
        if (this->shouldIgnoreToken()) {
            m_iterator++;
            m_position++;
            continue;
        }

        bool isSeparator = isWordSeparator(m_iterator);

        if (isSeparator && !word.isEmpty()) {
            break;
        }

        QChar character = *m_iterator;

        // The isLetter() check makes sure to exclude non-letter characters
        // from the output. That way free-standing symbols that are not
        // declared word separators don't get falsely interpreted as a word.

        if (!isSeparator && m_iterator->isLetterOrNumber()) {
            word.append(character);
        }
        m_iterator++;
        m_position++;
    }

    return word;
}

bool TextIterator::shouldIgnoreToken()
{
    if (m_commentsEnabled) {
        return false;
    }

    QTextBlock block = m_originalCursor.document()->findBlock(m_position);
    int positionInBlock = m_position - block.position();
    for (Range<int> range : UserData::fromBlock(block).comments()) {
        if (range.contains(positionInBlock)) {
            return true;
        }
    }
    
    return false;
}

void TextIterator::setCommentsEnabled(bool enabled)
{
    m_commentsEnabled = enabled;
}

bool TextIterator::commentsEnabled()
{
    return m_commentsEnabled;
}
