#ifndef TEXTITERATOR_H
#define TEXTITERATOR_H

#include <QString>
#include <QSet>


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

        TextIterator(const QString text, const IterationType iterationType);
        TextIterator(const TextIterator& textIterator);
        TextIterator& operator++();
        TextIterator operator++(int);
        TextIterator& operator+=(int by);
        bool operator==(const TextIterator& other) const;
        bool operator!=(const TextIterator& other) const;

        const QString current() const;
        bool atEnd() const;

        //! Ignores any characters surrounded by the specified opening and
        //! closing token.
        void ignoreEnclosedBy(const QChar openingToken, const QChar closingToken);

    private:
        enum TextIteratorState {
            None = 0,
            IgnoreBlock = 1
        };

        const QString m_text;
        const IterationType m_iterationType;
        QString::const_iterator m_charIterator;
        QString::const_iterator m_charIteratorEnd;
        QString m_current;
        QSet<const QPair<const QChar, const QChar>> m_ignorePairs;
        TextIteratorState m_state;

        const QString nextChar();
        const QString nextWord();

        void updateState();
};

#endif // TEXTITERATOR_H
