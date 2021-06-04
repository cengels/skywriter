#include <set>
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>

#include "symbols.h"
#include "../Range.h"
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
    m_document(cursor.document()),
    m_text(cursor.selectedText()),
    m_iterator(m_text.constBegin()),
    m_iteratorEnd(m_text.constEnd()),
    m_documentPosition(cursor.selectionStart()),
    m_iterationType(iterationType),
    m_current(),
    m_commentsEnabled(true),
    m_ended(false)
{
    (*this)++;
}

TextIterator::TextIterator(const QTextBlock& block, const IterationType iterationType) :
    m_document(block.document()),
    m_text(block.text()),
    m_iterator(m_text.constBegin()),
    m_iteratorEnd(m_text.constEnd()),
    m_documentPosition(block.position()),
    m_iterationType(iterationType),
    m_current(),
    m_commentsEnabled(true),
    m_ended(false)
{
    (*this)++;
}

TextIterator::TextIterator(const TextIterator& textIterator) :
    m_document(textIterator.m_document),
    m_text(textIterator.m_text),
    m_iterator(textIterator.m_iterator),
    m_iteratorEnd(textIterator.m_iteratorEnd),
    m_documentPosition(textIterator.m_documentPosition),
    m_iterationType(textIterator.m_iterationType),
    m_current(textIterator.m_current),
    m_commentsEnabled(textIterator.m_commentsEnabled),
    m_ended(textIterator.m_ended) { }

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
            && this->m_iterator == other.m_iterator
            && this->m_text == other.m_text;
}

bool TextIterator::operator!=(const TextIterator& other) const
{
    return !(this->operator==(other));
}

const QStringView TextIterator::current() const
{
    return this->m_current;
}

bool TextIterator::atEnd() const
{
    return m_ended;
}

QStringView TextIterator::nextChar()
{
    do {
        if (m_iterator == m_iteratorEnd) {
            m_ended = true;
            return QStringView();
        }

        m_iterator++;
        m_documentPosition++;
    } while (this->shouldIgnoreToken());

    return QStringView(m_iterator, 1);
}

QStringView TextIterator::nextWord()
{
    if (m_iterator == m_iteratorEnd) {
        // We need this additional branch instead of just making
        // atEnd() return "m_iterator == m_iteratorEnd"
        // because the last iteration step would be
        // skipped otherwise in atEnd() based loops.
        m_ended = true;
        return QStringView();
    }

    // Iterate until it finds a word separator, as long as the current word
    // is not empty (which means that the iteration began with a word
    // separator). Abort if the end of the string is found.

    const QChar* start = nullptr;
    const QChar* end = nullptr;

    while (m_iterator != m_iteratorEnd) {
        if (this->shouldIgnoreToken()) {
            m_iterator++;
            m_documentPosition++;
            continue;
        }

        bool isSeparator = isWordSeparator(m_iterator);

        if (isSeparator && start != nullptr) {
            break;
        }

        // The isLetterOrNumber() check makes sure to exclude non-letter characters
        // from the output. That way free-standing symbols that are not
        // declared word separators don't get falsely interpreted as a word.

        const bool isLetterOrNumber = m_iterator->isLetterOrNumber();

        if (start == nullptr && !isSeparator && m_iterator->isLetterOrNumber()) {
            start = m_iterator;
        }

        m_iterator++;
        m_documentPosition++;

        if (isLetterOrNumber) {
            // This ensures that, if the word ends in characters which are neither
            // letters/numbers nor get flagged in isWordSeparator(), they are
            // excluded from the QStringView.
            end = m_iterator;
        }
    }

    if (start == nullptr) {
        // Usually happens if m_text is empty.

        return QStringView();
    }

    return QStringView(start, end);
}

bool TextIterator::shouldIgnoreToken()
{
    if (!m_commentsEnabled) {
        return false;
    }

    QTextBlock block = m_document->findBlock(m_documentPosition);
    int positionInBlock = m_documentPosition - block.position();
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
