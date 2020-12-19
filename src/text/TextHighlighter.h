#ifndef TEXTHIGHLIGHTER_H
#define TEXTHIGHLIGHTER_H

#include <QObject>
#include <QTextDocument>
#include <QTextBlock>
#include <private/qquicktextnode_p.h>

#include "../Range.h"

//! The TextHighlighter is responsible for highlighting parts
//! of a QTextDocument based on external properties and syntax.
//! It does not directly operate on the QTextDocument, instead
//! painting its highlighting directly onto the canvas when requested.
//! This makes the TextHighlighter's performance a lot better when
//! compared to TextFormatter, at the cost of not being able
//! to fully customize the format.
class TextHighlighter : public QObject
{
    Q_OBJECT

    public:
        TextHighlighter(QTextDocument *parent);

        const QVector<Range<int>>& findRanges() const;
        void setFindRanges(const QVector<Range<int>>& ranges);

        void highlightBlock(QQuickTextNode& documentNode, const QTextBlock& block);
        void startHighlighting(int atY);

    Q_SIGNALS:
        void needsRepaint();

    private:
        QVector<Range<int>> m_findRanges;
        QVector<Range<int>>::const_iterator m_findRangeIterator;
        int m_atY;
};

#endif // TEXTHIGHLIGHTER_H
