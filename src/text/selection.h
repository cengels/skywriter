#ifndef SELECTION_H
#define SELECTION_H

#include <QTextCursor>

namespace selection
{
    //! Selects a word in the given QTextCursor. This function differs from
    //! QTextCursor::select(QTextCursor::WordUnderCursor) in that this
    //! function will stop expanding the selection if it reaches any non-letter
    //! token (such as quotes), whereas QTextCursor::select() will only stop
    //! on a number of predefined tokens like periods and commas.
    void selectWord(QTextCursor& cursor);
    void selectPreviousWord(QTextCursor& cursor, const QTextCursor::MoveMode mode = QTextCursor::MoveAnchor, const bool ignoreBlockBoundaries = true);
    void selectNextWord(QTextCursor& cursor, const QTextCursor::MoveMode mode = QTextCursor::MoveAnchor, const bool ignoreBlockBoundaries = true);
};

#endif // SELECTION_H
