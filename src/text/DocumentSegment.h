#ifndef DOCUMENTSEGMENT_H
#define DOCUMENTSEGMENT_H

#include <QObject>
#include <QTextDocument>
#include <QtConcurrent>
#include "FormattableTextArea/FormattableTextArea.h"

class DocumentSegment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(int wordCount READ wordCount NOTIFY wordCountChanged)
    Q_PROPERTY(int totalWordCount READ totalWordCount NOTIFY totalWordCountChanged)
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

        int length() const;
        //! Sets the length of this document segment by moving all subsequent document segments.
        void setLength(int length);

        //! Gets the text contained in this DocumentSegment.
        QString text() const;
        //! Gets the number of words of the text within this DocumentSegment.
        int wordCount() const;
        //! Gets the number of words of the text within this DocumentSegment and its children.
        int totalWordCount() const;

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
        DocumentSegment* parentSegment() const;

        //! Gets the QTextDocument this DocumentSegment corresponds to.
        QTextDocument* document() const;

        //! Returns a value indicating whether the DocumentSegment is valid.
        //! A DocumentSegment is valid if its position and depth is within
        //! bounds of the document.
        bool isValid() const;

        //! Gets the first text block contained in the DocumentSegment.
        QTextBlock firstBlock() const;
        //! Gets the last text block contained in the DocumentSegment.
        QTextBlock lastBlock() const;

        //! Updates the word count of the DocumentSegment.
        void updateWordCount();
        void updateTotalWordCount();

        bool operator==(const DocumentSegment& other) const;
        bool operator==(const DocumentSegment* other) const;
        bool operator!=(const DocumentSegment& other) const;
        bool operator!=(const DocumentSegment* other) const;

    Q_SIGNALS:
        void positionChanged();
        void textChanged();
        void wordCountChanged();
        void totalWordCountChanged();

    private:
        int m_position;
        int m_depth;
        int m_wordCount;
        int m_totalWordCount;

        int index() const;
};

#endif // DOCUMENTSEGMENT_H
