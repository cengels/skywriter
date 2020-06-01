#ifndef TEXTHIGHLIGHTER_H
#define TEXTHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

class TextHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    public:
        TextHighlighter(QTextDocument *parent);

        bool refreshing() const;

        void refresh();

    Q_SIGNALS:
        void commentsChanged();

    protected:
        void highlightBlock(const QString &text) override;

    private:
        enum BlockState {
            None = 0,
            EndsWithUnclosedComment = 1
        };

        void setCurrentBlockStateFlag(BlockState state);
        bool checkPreviousBlockStateFlag(BlockState state) const;

        void highlightComments(const QString &text);
        void highlightHeadings();

        bool m_refreshing;
};

#endif // TEXTHIGHLIGHTER_H
