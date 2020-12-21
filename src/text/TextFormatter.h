#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

#include "format.h"
#include "FormattableTextArea/FormattableTextArea.h"
#include "../Range.h"

//! The TextFormatter is responsible for formatting a QTextDocument's
//! text according to syntax. It inherits from QSyntaxHighlighter and
//! directly changes the formats of the QTextDocument.
class TextFormatter : public QSyntaxHighlighter
{
    Q_OBJECT

    public:
        TextFormatter(QTextDocument *parent);

        bool refreshing() const;
        void refresh();

    Q_SIGNALS:
        void commentsChanged();

    protected:
        void highlightBlock(const QString &text) override;

    private:
        void setCurrentBlockStateFlag(format::BlockState state);
        void unsetCurrentBlockStateFlag(format::BlockState state);
        bool checkPreviousBlockStateFlag(format::BlockState state) const;
        bool checkCurrentBlockStateFlag(format::BlockState state) const;

        //! Applies heading format to headings.
        void formatHeadings();
        //! Applies comment format to comments.
        void formatComments(const QString& text);

        void setCharFormat(int blockPosition, int length, const QTextCharFormat& format);
        void setColor(int blockPosition, int length, const QColor& color);
        void setBackgroundColor(int blockPosition, int length, const QColor& color);
        void setBlockFormat(const QTextCharFormat& format);
        //! The way the QSyntaxHighlighter works is that it stores a list of
        //! formats to be applied at the end of a rehighlight pass every time
        //! you call setFormat(). However, since these formats are not actually
        //! applied yet until the end of the rehighlight, two setFormats()
        //! that cut across the same range will override each other, not merge.
        //! There is no way to retrieve a highlighter format at a given position
        //! so we have to store them separately and then apply them at the end
        //! of each highlightBlock().
        void mergeFormats();

        bool m_refreshing;
        const QVector<Range<int>>* m_findRanges;
        QVector<QTextLayout::FormatRange> m_formats;
};

#endif // TEXTFORMATTER_H
