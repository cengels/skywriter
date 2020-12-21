#include "UserData.h"

UserData::UserData()
{ }

UserData& UserData::fromBlock(QTextBlock& block) {
    UserData* userData = dynamic_cast<UserData*>(block.userData());

    if (!userData) {
        userData = new UserData();
        block.setUserData(userData);
    }

    return *userData;
}
