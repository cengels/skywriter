#ifndef TEXTHIGHLIGHTER_H
#define TEXTHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

class TextHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    Q_PROPERTY(int comments READ comments MEMBER m_comments NOTIFY commentsChanged);

    public:
        TextHighlighter(QTextDocument *parent);

        int comments() const;
        bool refreshing() const;

        void refresh();

    Q_SIGNALS:
        void commentsChanged();

    protected:
        void highlightBlock(const QString &text) override;

    private:
        int m_comments;
        bool m_refreshing;
};

#endif // TEXTHIGHLIGHTER_H
