#ifndef USERDATA_H
#define USERDATA_H

#include <QTextBlockUserData>
#include "../Range.h"


struct UserData : public QTextBlockUserData
{
    public:
        UserData();
        QVector<Range<int>>& searchMatches();

    private:
        QVector<Range<int>> m_searchMatches;
};

#endif // USERDATA_H
