#ifndef TEXTHIGHLIGHTER_H
#define TEXTHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

#include "format.h"
#include "FormattableTextArea/FormattableTextArea.h"

class TextHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    Q_PROPERTY(QString sceneBreak READ sceneBreak WRITE setSceneBreak NOTIFY sceneBreakChanged)

    public:
        TextHighlighter(QTextDocument *parent);

        const QString& sceneBreak() const;
        void setSceneBreak(const QString& sceneBreakString);

        bool refreshing() const;

        void refresh();

    Q_SIGNALS:
        void commentsChanged();
        void sceneBreakChanged();

    protected:
        void highlightBlock(const QString &text) override;

    private:
        void setCurrentBlockStateFlag(format::BlockState state);
        void unsetCurrentBlockStateFlag(format::BlockState state);
        bool checkPreviousBlockStateFlag(format::BlockState state) const;
        bool checkCurrentBlockStateFlag(format::BlockState state) const;

        void highlightComments(const QString& text);
        void highlightHeadings();
        void highlightSceneBreaks(const QString& text);
        void highlightMatches();

        void setBlockFormat(const QTextCharFormat& format);

        bool m_refreshing;
        QString m_sceneBreakString;
};

#endif // TEXTHIGHLIGHTER_H
