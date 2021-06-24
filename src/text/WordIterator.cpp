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
#include "WordIterator.h"

namespace {
    qsizetype find_first_of(const QString& string, const QSet<QChar>& set, qsizetype from = 0) {
        qsizetype smallest = -1;

        for (const QChar& character : set) {
            qsizetype result = string.indexOf(character, from);

            if (result != -1 && (smallest == -1 || result < smallest)) {
                smallest = result;
            }
        }

        return smallest;
    }
}

// A previous approach considered using a copied QTextCursor instead of m_iterator here to iterate
// through the text. However, each call of QTextCursor::movePosition() or QTextCursor::setPosition()
// takes a significant period of time. A simple iterator is much faster.
// We also need to make sure that we use iterators to a copy of the selectedText, not to a reference
// otherwise everything breaks.
WordIterator::WordIterator(const QString& string) :
    m_text(string),
    m_position(0),
    m_atEnd(false),
    m_startOfWord(0) { }

WordIterator::WordIterator(const WordIterator& iterator) :
    m_text(iterator.m_text),
    m_position(iterator.m_position),
    m_atEnd(iterator.m_atEnd),
    m_startOfWord(iterator.m_startOfWord) { }

WordIterator WordIterator::operator++(int)
{
    WordIterator returnValue = *this;
    this->operator++();
    return returnValue;
}

WordIterator& WordIterator::operator+=(int by)
{
    for (int i = 0; i < by && !atEnd(); i++) {
        this->operator++();
    }

    return *this;
}

bool WordIterator::operator==(const WordIterator& other) const
{
    return this->m_position == other.m_position
        && this->m_text == other.m_text
        && this->m_startOfWord == other.m_startOfWord;
}

bool WordIterator::operator!=(const WordIterator& other) const
{
    return !(this->operator==(other));
}

const QString WordIterator::current() const
{
    return this->m_text.sliced(this->m_startOfWord, this->m_position);
}

bool WordIterator::atEnd() const
{
    return this->m_atEnd;
}

WordIterator& WordIterator::operator++()
{
    // Iterate until it finds a word separator, as long as the current word
    // is not empty (which means that the iteration began with a word
    // separator). Abort if the end of the string is found.

    // Examples: (iterator stops at: ^)
    // test one two three
    //      ^   ^   ^
    // ––––why—is you—here
    //         ^  ^   ^

    if (this->atEnd()) {
        return *this;
    }

    qsizetype separator_pos = find_first_of(this->m_text, symbols::word_separators_all, this->m_position);

    while (separator_pos != -1) {
        int m_position_debug = this->m_position;
        int m_startOfWord_debug = this->m_startOfWord;

        // If the iterator is currently on a word separator,
        // advance the iterator by one character and try again.
        if (separator_pos == this->m_position) {
            this->m_position++;
            separator_pos = find_first_of(this->m_text, symbols::word_separators_all, this->m_position);
            continue;
        }

        // If the iterator is currently on a word_separator_multiple,
        // check if the next or previous character is also a word_separator_multiple,
        // else try again.
        const QChar character = this->m_text[separator_pos];
        const bool isMulti = symbols::word_separators_multiple.contains(character);

        if (isMulti) {
            if ((separator_pos + 1 >= this->m_text.size() || this->m_text[separator_pos + 1] != character)
             && (separator_pos - 1 < 0 || this->m_text[separator_pos - 1] != character)) {
                separator_pos = find_first_of(this->m_text, symbols::word_separators_all, separator_pos + 1);
                continue;
            }
        }

        // At this point separator_pos will point to the next word separator
        this->m_startOfWord = this->m_position;
        this->m_position = separator_pos - 1;

        if (this->m_startOfWord != this->m_position) {
            // Maybe valid word, exit
            return *this;
        }

        separator_pos = find_first_of(this->m_text, symbols::word_separators_all, this->m_position + 1);
        this->m_position = separator_pos;
    }

    this->m_atEnd = true;

    return *this;
}
