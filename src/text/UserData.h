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

    private:
};

#endif // USERDATA_H
