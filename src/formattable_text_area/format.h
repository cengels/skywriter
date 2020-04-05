#ifndef FORMAT_H
#define FORMAT_H

#include <QTextCursor>

namespace format {
    const QTextCharFormat getMergedCharFormat(const QTextCursor& textCursor);
}

#endif // FORMAT_H
