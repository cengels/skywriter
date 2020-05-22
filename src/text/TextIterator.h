#ifndef TEXTITERATOR_H
#define TEXTITERATOR_H

#include <QString>


class TextIterator
{
    public:
        enum IterationType {
            ByCharacter,
//            ByLetter,
            ByWord
//            BySentence,
//            ByLine
        };

        TextIterator(const QString& text, const IterationType iterationType);
        TextIterator(const TextIterator& textIterator);
        TextIterator& operator++();
        TextIterator operator++(int);
        TextIterator& operator+=(int by);
        bool operator==(const TextIterator& other) const;
        bool operator!=(const TextIterator& other) const;

        const QString current() const;
        bool atEnd() const;

    private:
        const QString& m_text;
        const IterationType m_iterationType;
        QString::const_iterator m_charIterator;
        QString::const_iterator m_charIteratorEnd;
        QString m_current;
};

#endif // TEXTITERATOR_H
