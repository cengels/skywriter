#ifndef WORDITERATOR_H
#define WORDITERATOR_H

#include <QString>

class WordIterator
{
    public:
        WordIterator(const QString& string);
        WordIterator(const WordIterator& iterator);
        WordIterator& operator++();
        WordIterator operator++(int);
        WordIterator& operator+=(int by);
        bool operator==(const WordIterator& other) const;
        bool operator!=(const WordIterator& other) const;

        const QString current() const;
        bool atEnd() const;

    private:
        const QString m_text;
        qsizetype m_startOfWord;
        qsizetype m_position;
        qsizetype m_atEnd;
};

#endif // WORDITERATOR_H
