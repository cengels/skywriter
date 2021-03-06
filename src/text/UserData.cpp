#include "UserData.h"

UserData::UserData() : m_wordCount(0), m_comments()
{ }

UserData& UserData::fromBlock(QTextBlock& block) {
    UserData* userData = dynamic_cast<UserData*>(block.userData());

    if (!userData) {
        userData = new UserData();
        block.setUserData(userData);
    }

    return *userData;
}

int UserData::wordCount() const
{
    return this->m_wordCount;
}

void UserData::setWordCount(int wordCount)
{
    this->m_wordCount = wordCount;
}

void UserData::addCommentRange(int from, int to)
{
    m_comments.push_back(Range<int>(from, to));
}

void UserData::clearCommentRanges()
{
    m_comments.clear();
}

const QVector<Range<int>> UserData::comments() const
{
    return m_comments;
}
