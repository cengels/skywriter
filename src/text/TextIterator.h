#ifndef TEXTITERATOR_H
#define TEXTITERATOR_H

#include <QString>
#include <QStringView>
#include <QSet>
#include <QTextCursor>

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

        TextIterator(const QTextCursor& cursor, const IterationType iterationType);
        TextIterator(const QTextBlock& block, const IterationType iterationType);
        TextIterator(const TextIterator& textIterator);
        TextIterator& operator++();
        TextIterator operator++(int);
        TextIterator& operator+=(int by);
        bool operator==(const TextIterator& other) const;
        bool operator!=(const TextIterator& other) const;

        const QStringView current() const;
        bool atEnd() const;

        //! If set to true, will not count comments.
        void setCommentsEnabled(bool enabled);
        //! Whether the TextIterator should count comments. False by default.
        bool commentsEnabled();

    private:
        const QTextDocument* m_document;
        QString m_text;
        QString::const_iterator m_iterator;
        QString::const_iterator m_iteratorEnd;
        int m_documentPosition;
        const IterationType m_iterationType;
        QStringView m_current;
        bool m_commentsEnabled;
        bool m_ended;

        QStringView nextChar();
        QStringView nextWord();
        bool shouldIgnoreToken();
};

#endif // TEXTITERATOR_H
