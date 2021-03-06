#ifndef USERDATA_H
#define USERDATA_H

#include <QTextBlockUserData>
#include "../Range.h"

struct UserData : public QTextBlockUserData
{
    public:
        UserData();

        //! Gets the UserData instance from the specified text block.
        //! If it doesn't exist, constructs a new instance and attaches
        //! it to the text block before returning it.
        static UserData& fromBlock(QTextBlock& block);

        int wordCount() const;
        void setWordCount(int wordCount);

        void addCommentRange(int from, int to);
        void clearCommentRanges();
        const QVector<Range<int>> comments() const;

    private:
        int m_wordCount;
        QVector<Range<int>> m_comments;
};

#endif // USERDATA_H
