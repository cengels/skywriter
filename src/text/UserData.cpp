#include "UserData.h"

UserData::UserData() : m_searchMatches()
{

}

QVector<Range<int>>& UserData::searchMatches()
{
    return m_searchMatches;
}
