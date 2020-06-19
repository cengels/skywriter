#ifndef DOCUMENTSEGMENT_H
#define DOCUMENTSEGMENT_H

#include <QObject>
#include <QTextDocument>
#include "FormattableTextArea/FormattableTextArea.h"

class DocumentSegment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(int words READ words NOTIFY wordsChanged)
    Q_PROPERTY(QString heading READ heading CONSTANT)
    Q_PROPERTY(QString subheading READ subheading CONSTANT)
    Q_PROPERTY(int depth READ depth CONSTANT)

    public:
        explicit DocumentSegment(QObject* parent = nullptr);
        DocumentSegment(int position, int depth, QObject* parent);

        //! Gets the text index where the DocumentSegment begins.
        //! The DocumentSegment runs until the the next DocumentSegment's
        //! position() or until the end of the document.
        int position() const;
        void setPosition(int position);

        //! Gets the text contained in this DocumentSegment.
        QString text() const;
        //! Gets the number of words of the text within this DocumentSegment.
        int words() const;

        //! Gets the heading of this DocumentSegment. A new DocumentSegment
        //! begins at a new heading, so each DocumentSegment must, by definition,
        //! have a heading.
        QString heading() const;
        //! Gets the subheading of this DocumentSegment. While a heading is
        //! mandatory, a subheading is not. A DocumentSegment only has a
        //! subheading if another heading of a higher depth immediately
        //! follows its primary heading.
        QString subheading() const;
        //! Gets the depth of the DocumentSegment. Note that this does not necessarily
        //! correspond to the heading level as headings and subheadings are
        //! combined.
        int depth() const;

        DocumentSegment* next() const;
        DocumentSegment* previous() const;

        //! Gets the QTextDocument this DocumentSegment corresponds to.
        QTextDocument* document() const;

        //! Returns a value indicating whether the DocumentSegment is valid.
        //! A DocumentSegment is valid if its position and depth is within
        //! bounds of the document.
        bool isValid() const;

    Q_SIGNALS:
        void positionChanged();
        void textChanged();
        void wordsChanged();

    private:
        int m_position;
        int m_depth;
        int m_words;

        int index() const;
        void updateWords();
};

#endif // DOCUMENTSEGMENT_H
