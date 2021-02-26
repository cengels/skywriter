#include "UserData.h"

UserData::UserData() : m_wordCount(0)
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
