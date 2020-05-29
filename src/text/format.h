#ifndef FORMAT_H
#define FORMAT_H

#include <QTextCursor>

//! Contains convenience methods related to text formatting.
namespace format {
    //! Gets a new QTextCharFormat containing all the formats found in the text
    //! cursor's selection, or at its position if no text is selected.
    //! This function only returns the value if the entire range is styled.
    //! For instance, if the cursor selects a range where half the range is
    //! italicized and half the range is not, the resulting format will have
    //! fontItalics() set to false.
    const QTextCharFormat getMergedCharFormat(const QTextCursor& textCursor);
}

#endif // FORMAT_H
